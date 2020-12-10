//compilation instructions/examples:
//gcc -fopenmp point_epsilon_starter.c -o point_epsilon_starter
//sometimes you need to link against the math library with -lm:
//gcc -fopenmp point_epsilon_starter.c -lm -o point_epsilon_starter

//math library needed for the square root

#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "omp.h"

//N is 100000 for the submission. However, you may use a smaller value of testing/debugging.
#define N 100000
//Do not change the seed, or your answer will not be correct
#define SEED 72

struct pointData{
double x;
double y;
};

void generateDataset(struct pointData * data);
double calcSquare(double arg);
double difference(double arg1, double arg2);


int main(int argc, char *argv[])
{
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
	printf("\nSize of dataset (MiB): %f",(2.0*sizeof(double)*N*1.0)/(1024.0*1024.0));
	generateDataset(data);

	//change OpenMP settings:
omp_set_num_threads(1);

  double epsilonSquared = calcSquare(epsilon);
  int i;
  int j;
  int total = N;

   double tstart=omp_get_wtime();

     #pragma omp parallel for shared(data) private(i,j) reduction(+: total)
     for(i = 0; i < N - 1; i++)
     {     
        for(j = i + 1; j < N; j++)
        {
   	   double differenceX = data[i].x - data[j].x;
	   double differenceY = data[i].y - data[j].y;
           
	    if( ( abs(differenceX * differenceX) + abs(differenceY * differenceY) ) <= epsilonSquared )
            {
	       total+=2;
            }
         }
      }
	double tend=omp_get_wtime();

	printf("\nTotal time (s): %f",tend-tstart);
  printf("\nN = %d, epsilon = %f, total count: %d", N, epsilon, total);

	free(data);
	printf("\n");
	return 0;
}

// calculate square of two double arguments
double calcSquare(double arg)
{
  return arg * arg;
}

// calculate difference between two doubles
double difference(double arg1, double arg2)
{
  return abs(arg1 - arg2);
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
