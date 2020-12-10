#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>




pthread_mutex_t lock;
int count;


void *do_work(void *arg);



int main(int argc, char **argv) {
  pthread_mutex_init(&lock, NULL);
  count=0;
  int num_threads=100;
  pthread_t worker_thread[num_threads];

  // Creating child threads
  int t;
  for (t=0; t < num_threads; t++) {
    if (pthread_create(&worker_thread[t], NULL, 
                      do_work, (void *) (long int)t)) {
      fprintf(stderr,"Error while creating thread #%d\n",t);
      exit(1);
    }

    fprintf(stdout,"Waiting for child thread to complete\n");
  }
  
  // Joining with child threads
  for (t=0; t < num_threads; t++) {
    if (pthread_join(worker_thread[t], NULL)) {
      fprintf(stderr,"Error while joining with child thread #%d\n",t);
      exit(1);
    }
  }

  printf("\nCount: %d (should be: %d)", count, ((num_threads-1)*(num_threads))/2);
  printf("\n\n");
  exit(0); 
}


void *do_work(void *arg) {
  
  long int tid=(long int)arg;
  
  pthread_mutex_lock(&lock);
  count+=tid;
  pthread_mutex_unlock(&lock);
  

  return NULL;
}
