


#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <omp.h>
#include <iostream>

#define N 900000000 //min:10 max: 900000000 (unsigned int will reach maximum size)

using namespace std;

void warmUpGPU();
__global__ void vectorAdd(unsigned int * A, unsigned int * B, unsigned int * C);

int main(int argc, char *argv[])
{
	
	warmUpGPU();
	
	//change OpenMP settings:
	omp_set_num_threads(1);


	unsigned int * A;
	unsigned int * B;
	unsigned int * C;
	unsigned int * C_CPU;

	A=(unsigned int *)malloc(sizeof(unsigned int)*N);
	B=(unsigned int *)malloc(sizeof(unsigned int)*N);
	C=(unsigned int *)malloc(sizeof(unsigned int)*N);
	C_CPU=(unsigned int *)malloc(sizeof(unsigned int)*N);


	printf("\nSize of A+B+C (GiB): %f",(sizeof(unsigned int)*N*3.0)/(1024.0*1024.0*1024.0));
	

	//init:
	unsigned int i=0;
	for (i=0; i<N; i++){
		A[i]=i;
		B[i]=i;
		C[i]=0;
		C_CPU[i]=0;
	}


	//CPU version:
	double tstartCPU=omp_get_wtime();
	
	for (int i=0; i<N; i++){
		C_CPU[i]=A[i]+B[i];
	}
	double tendCPU=omp_get_wtime();

	printf("\nTime CPU (s): %f",tendCPU - tstartCPU);

	

	double tstart=omp_get_wtime();

	
	//CUDA error code:
	
	cudaError_t errCode=cudaSuccess;
	
	if(errCode != cudaSuccess)
	{
		cout << "\nLast error: " << errCode << endl; 	
	}

	unsigned int * dev_A;
	unsigned int * dev_B;
	unsigned int * dev_C;

	//allocate on the device: A, B, C
	errCode=cudaMalloc((unsigned int**)&dev_A, sizeof(unsigned int)*N);	
	if(errCode != cudaSuccess) {
	cout << "\nError: A error with code " << errCode << endl; 
	}

	errCode=cudaMalloc((unsigned int**)&dev_B, sizeof(unsigned int)*N);	
	if(errCode != cudaSuccess) {
	cout << "\nError: B error with code " << errCode << endl; 
	}

	errCode=cudaMalloc((unsigned int**)&dev_C, sizeof(unsigned int)*N);	
	if(errCode != cudaSuccess) {
	cout << "\nError: C error with code " << errCode << endl; 
	}

	//copy A to device
	errCode=cudaMemcpy( dev_A, A, sizeof(unsigned int)*N, cudaMemcpyHostToDevice);
	if(errCode != cudaSuccess) {
	cout << "\nError: A memcpy error with code " << errCode << endl; 
	}	
	
	//copy B to device
	errCode=cudaMemcpy( dev_B, B, sizeof(unsigned int)*N, cudaMemcpyHostToDevice);
	if(errCode != cudaSuccess) {
	cout << "\nError: A memcpy error with code " << errCode << endl; 
	}

	//copy C to device (initialized to 0)
	errCode=cudaMemcpy( dev_C, C, sizeof(unsigned int)*N, cudaMemcpyHostToDevice);
	if(errCode != cudaSuccess) {
	cout << "\nError: A memcpy error with code " << errCode << endl; 
	}

	//execute kernel
	const unsigned int totalBlocks=ceil(N*1.0/1024.0);
	printf("\ntotal blocks: %d",totalBlocks);
	vectorAdd<<<totalBlocks,1024>>>(dev_A, dev_B, dev_C);

	if(errCode != cudaSuccess){
		cout<<"Error afrer kernel launch "<<errCode<<endl;
	}

	//copy data from device to host 
	errCode=cudaMemcpy( C, dev_C, sizeof(unsigned int)*N, cudaMemcpyDeviceToHost);
	if(errCode != cudaSuccess) {
	cout << "\nError: getting C result form GPU error with code " << errCode << endl; 
	}



	cudaDeviceSynchronize();

	//testing
	for (int i=N-10; i<N; i++)
	{
		printf("\n%d",C[i]);
	}
	
	
	double tend=omp_get_wtime();
	
	printf("\nTotal time GPU (s): %f",tend-tstart);



	printf("\n");

	return 0;
}

__global__ void vectorAdd(unsigned int * A, unsigned int * B, unsigned int * C) {

unsigned int tid=threadIdx.x+ (blockIdx.x*blockDim.x); 

if (tid>=N){
	return;
}
C[tid]=A[tid]+B[tid];

return;
}

























__global__ void warmup(unsigned int * tmp) {
if (threadIdx.x==0)
*tmp=555;

return;
}



void warmUpGPU(){


printf("\nWarming up GPU for time trialing...\n");	
cudaDeviceSynchronize();

return;
}