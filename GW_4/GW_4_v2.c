#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <omp.h>

#define N 40000
#define THRESHOLD 1.0

#define NTHREADS 4

void populatedata(double * x, double * y);
double expodist(double lambda);
double computedistance(double * a, double * b);
int main(int argc, char **argv) {

  //do not change
  srand(26);

  omp_set_num_threads(NTHREADS);

  double * x=(double *)malloc(sizeof(double)*N);
  double * y=(double *)malloc(sizeof(double)*N);
  int * z=(int *)malloc(sizeof(int)*N);
  printf("\nAllocated %f GiB", ((sizeof(double)*N*2.0)+(sizeof(int)*N*1.0))/(1024*1024*1024.0));
  populatedata(x, y);

  //Write code here

  int i,j,count;
  i = 0;
  j = 0;
  count = 0;
  double tend;
  double tstart=omp_get_wtime();

   #pragma omp parallel shared(x,y,z,i) private(j)
   {
      while(i < N)
      {
         for(j=0; j<N; j++)
         {
            if(computedistance(&x[i], &y[j]) <= THRESHOLD)
            {
               #pragma omp atomic
               z[i]++;
            }
         }

         #pragma omp atomic
         i++;
      }
   }

   tend = omp_get_wtime();

   for(i=0; i<N; i++)
   {
      count += z[i];
   }

  printf("\nElapsed time: %f seconds\n", tend-tstart);
  printf("\nTotal distance value (sum of z vals): %d\n", count);

  free(x);
  free(y);
  free(z);
  printf("\n\n");
  return 0;
}

double computedistance(double * a, double * b)
{

  return(fabs((*a)-(*b)));
}

void populatedata(double * x, double * y)
{
  double lambda=0.5;
  int i;
  for (i=0; i<N; i++)
  {
    x[i]=expodist(lambda);
    y[i]=expodist(lambda);

  }
}




double expodist(double lambda)
{
    double u;
    u = rand() / (RAND_MAX + 1.0);
    return -log(1- u) / lambda;
}
