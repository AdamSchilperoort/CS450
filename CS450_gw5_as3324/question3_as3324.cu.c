#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <omp.h>


#define N 100

using namespace std;

void warmUpGPU();
__global__ void vectorAdd(unsigned int * A, unsigned int * B, unsigned int * C);

int main(int argc, char *argv[])
{

        warmUpGPU();

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
        int i=0;
        for (i=0; i<N; i++){
                A[i]=i;
                B[i]=i;
                C[i]=0;
                C_CPU[i]=0;
        }

        double tstart = omp_get_wtime();
        double t_CPUstart = omp_get_wtime();


        //CPU version:


        for (int i=0; i<N; i++){
                C_CPU[i]=A[i]+B[i];
        }

        double t_CPUend = omp_get_wtime();


        //CUDA error code:
        cudaError_t errCode=cudaSuccess;

        unsigned int * dev_A;
        unsigned int * dev_B;
        unsigned int * dev_C;

        double t_GPUstart = omp_get_wtime();

        //allocate on the device: A, B, C
        errCode=cudaMalloc((unsigned int**)&dev_A, sizeof(unsigned int)*N);
        if(errCode != cudaSuccess) {
        cout << "\nError: A error with code " << errCode << endl;
        }

	if(errCode != cudaSuccess) {
        cout << "\nError: B error with code " << errCode << endl;
        }

        errCode=cudaMalloc((unsigned int**)&dev_C, sizeof(unsigned int)*N);
        if(errCode != cudaSuccess) {
        cout << "\nError: C error with code " << errCode << endl;
        }


        double t_transferTOstart = omp_get_wtime();

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

        double t_transferTOend = omp_get_wtime();


        double t_GPUkernelstart = omp_get_wtime();

        //execute kernel
        const unsigned int totalBlocks=ceil(N*1.0/1024.0);
        printf("\ntotal blocks: %d",totalBlocks);
        vectorAdd<<<totalBlocks,1024>>>(dev_A, dev_B, dev_C);

        cudaDeviceSynchronize();

        if(errCode != cudaSuccess){
                cout<<"Error after kernel launch "<<errCode<<endl;
        }
         double t_GPUkernelend = omp_get_wtime();

        double t_transferFROMstart = omp_get_wtime();

	 //copy data from device to host
        errCode=cudaMemcpy( C, dev_C, sizeof(unsigned int)*N, cudaMemcpyDeviceToHost);
        if(errCode != cudaSuccess) {
        cout << "\nError: getting C result form GPU error with code " << errCode << endl;
        }

        double t_GPUend = omp_get_wtime();

        double t_transferFROMend = omp_get_wtime();

        cudaDeviceSynchronize();

        double tend = omp_get_wtime();

        printf("\nTotal time(s): %f", tend-tstart);

        printf("\nCPU Time(s): %f", t_CPUend - t_CPUstart);

        printf("\nTotal GPU time(s): %f", t_GPUend - t_GPUstart);

        printf("\nTransfer data GPU time(s): %f", (t_transferFROMend - t_transferFROMstart) + (t_transferTOend - t_transferTOstart));

        printf("\nGPU kernel time(s): %f", t_GPUkernelend - t_GPUkernelstart);

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




void warmUpGPU(){
cudaDeviceSynchronize();
return;
}
