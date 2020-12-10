#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <omp.h>
#include <iostream>

//See values of N in assignment instructions.

#define N 1000
//Do not change the seed, or your answer will not be correct
#define SEED 72

//For GPU implementation
#define BLOCKSIZE 1024

using namespace std;

struct pointData{
double x;
double y;
};

//void warmUpGPU();
void generateDataset(struct pointData * data);
__global__ void calcDistance(struct pointData * pointData, unsigned int * count, double * epsilon);

int main(int argc, char *argv[])
{

        //warmUpGPU();

        //Read epsilon distance from command line
        if (argc!=2)
        {
        printf("\nIncorrect number of input parameters. Please input an epsilon distance.\n");
        return 0;
        }


        char inputEpsilon[20];
        strcpy(inputEpsilon,argv[1]);
        double epsilon=atof(inputEpsilon);


        //generate dataset:
        struct pointData * data;
        data=(struct pointData*)malloc(sizeof(struct pointData)*N);
        printf("\nSize of dataset (MiB): %f",(2.0*sizeof(double)*N*1.0)/(BLOCKSIZE * BLOCKSIZE));
        generateDataset(data);

        omp_set_num_threads(1);

        cudaError_t errCode=cudaSuccess;

        struct pointData * dev_pointData;
        double dev_epsilon;

        // start time for total time (including memory allocation and copying)
        double tstart=omp_get_wtime();

        // allocate on the device
        errCode=cudaMalloc((struct pointData**)&dev_pointData, sizeof(struct pointData)*N);
        if(errCode != cudaSuccess) {
        cout << "\nError: A error with code " << errCode << endl;
        }
        // copy  to the device
        errCode=cudaMemcpy( dev_pointData, data, sizeof(struct pointData)*N, cudaMemcpyHostToDevice);
        if(errCode != cudaSuccess) {
        cout << "\nError: A memcpy error with code " << errCode << endl;
        }

        // copy epsilon over
        /*
        errCode=cudaMalloc((double)dev_epsilon, sizeof(double));
        if(errCode != cudaSuccess) {
        cout << "\nError: B error with code " << errCode << endl;
        }

        errCode=cudaMemcpy( &dev_epsilon, epsilon, sizeof(double), cudaMemcpyHostToDevice);
        if(errCode != cudaSuccess) {
        cout << "\nError: B memcpy error with code " << errCode << endl;
        }
        */

        unsigned int * count;
        unsigned int * dev_count;

        count=(unsigned int *)malloc(sizeof(unsigned int));
        dev_count=(unsigned int *)malloc(sizeof(unsigned int));
        *count = 0;

        //copy count over
        errCode=cudaMalloc((unsigned int**)&dev_count, sizeof(unsigned int));
        if(errCode != cudaSuccess) {
        cout << "\nError: C cudaMalloc error with code " << errCode << endl;
        }
        
        errCode=cudaMemcpy( dev_count, count, sizeof(unsigned int), cudaMemcpyHostToDevice);
        if(errCode != cudaSuccess) {
        cout << "\nError: C memcpy error with code " << errCode << endl;
        }


        //cudaDeviceSynchronize()


        // execute the kernel
        // time the kernel
        double tKernelStart=omp_get_wtime();

        const unsigned int totalBlocks=ceil(N*1.0/BLOCKSIZE);
        printf("\ntotal blocks: %d",totalBlocks);

        calcDistance<<<totalBlocks,BLOCKSIZE>>>(dev_pointData, dev_count, epsilon);

        cudaDeviceSynchronize();

        double tKernelEnd=omp_get_wtime();


        //copy data from device to host
        errCode=cudaMemcpy( count, dev_count, sizeof(unsigned int), cudaMemcpyDeviceToHost);
        if(errCode != cudaSuccess) {
        cout << "\nError: getting result form GPU error with code " << errCode << endl;
        }

        printf("\nTotal count: %d",*count);

        // get total time
        double tend=omp_get_wtime();

        printf("\nTotal time (s): %f",tend-tstart);
        printf("\nGPU Kernel time (s): %f",tKernelEnd-tKernelStart);

        free(data);
        printf("\n");
        return 0;
}

__global__ void calcDistance(struct pointData * data, unsigned int * count, double epsilon)
{
        //unsigned int tid=threadIdx.x+ (blockIdx.x*blockDim.x);
        unsigned int tid = threadIdx.x + BLOCKSIZE * blockIdx.x;

        if (tid>=N){
        return;
        }

        // attempt at shared memory
        __shared__ double sharedEpsilon_squared;
        sharedEpsilon_squared = epsilon * epsilon;

        __shared__ struct pointData sharedData[BLOCKSIZE];
        // __shared__ struct pointData sharedData2[N]; <-- illegal...?

        //tile data from data to array
        sharedData[tid] = data[tid];
        //sharedData2[tid] = data[tid];

        __syncthreads();

        int i;

        for(i=0; i<N; i++)
        {
                if ( fabs( (sharedData[tid].x - sharedData[i].x ) * (sharedData[tid].x - sharedData[i].x) )
                   + fabs( (sharedData[tid].y - sharedData[i].y ) * (sharedData[tid].y - sharedData[i].y) )
                   <= ( sharedEpsilon_squared ) )
                {
                        atomicAdd(count, int(1));
                }
        }


        return;
}

//Do not modify the dataset generator or you will get the wrong answer
void generateDataset(struct pointData * data)
{

        //seed RNG
        srand(SEED);


        for (unsigned int i=0; i<N; i++){
                data[i].x=1000.0*((double)(rand()) / RAND_MAX);
                data[i].y=1000.0*((double)(rand()) / RAND_MAX);
        }


}
     
