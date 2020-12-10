#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void *do_work(void *arg);

pthread_mutex_t lock, lock_even, lock_odd;
pthread_cond_t is_empty;
pthread_cond_t is_full;
int buffer=0;
int buffer_size=0;
int num_updates_even=0;
int num_updates_odd=0;

int main(int argc, char *argv) {
  int t;

  int num_threads=10;
  pthread_t worker_thread[num_threads];
  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&is_empty, NULL);
  pthread_cond_init(&is_full, NULL);
  

  // Creating child threads
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
  exit(0); 

  printf("\n\n");
}

void *do_work(void *arg) {
  
  long int myTid=(long int )arg;
  
  //Code for even threads
  //Add to buffer
  if (myTid%2==0)
  { 
    pthread_mutex_lock(&lock);
    while (buffer_size==1)
    {
      fprintf(stderr,"\n[Tid %d] Waiting", myTid);
      pthread_cond_wait(&is_empty, &lock);
    }
    buffer=myTid;
    fprintf(stderr,"\n[Tid %d] Buffer is: %d", myTid, buffer);
    buffer_size++;
    pthread_cond_signal(&is_full);
    pthread_mutex_unlock(&lock);
    usleep(100000);
  }
  
  //Code for odd threads
  //Remove from buffer
  else if (myTid%2==1)
  {
    pthread_mutex_lock(&lock);
    while (buffer_size==0)
    {
      fprintf(stderr,"\n[Tid %d] Waiting", myTid);
      pthread_cond_wait(&is_full, &lock);
    }
    int myBuffer=buffer;
    fprintf(stderr,"\n[Tid %d] Buffer is: %d", myTid, myBuffer);
    buffer_size--;
    pthread_cond_signal(&is_empty);
    pthread_mutex_unlock(&lock);
    usleep(100000);
  }
  

  return NULL;
}
