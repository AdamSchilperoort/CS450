#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>


void *do_work(void *arg);

// create struct to store values
struct arguments {
   long int myTid;
   pthread_mutex_t lock, lock_even, lock_odd;
   pthread_cond_t is_empty;
   pthread_cond_t is_full;
   int *buffer;
   int *buffer_size;
   int *num_updates_even;
   int *num_updates_odd;
};



int main(int argc, char **argv) {
  int t;

  int num_threads = 10;
  int buffer = 0;
  int buffer_size = 0;
  int num_updates_even = 0;
  int num_updates_odd = 0;

  pthread_t worker_thread[num_threads];
    
  pthread_mutex_t lock, lock_even, lock_odd;
  pthread_cond_t is_full, is_empty;

  pthread_mutex_init(&lock, NULL);
  pthread_cond_init(&is_empty, NULL);
  pthread_cond_init(&is_full, NULL);
  pthread_mutex_init(&lock_even, NULL);
  pthread_mutex_init(&lock_odd, NULL);

  struct arguments *args[num_threads];


  for (t = 0; t < num_threads; t++) {
     args[t] = (struct arguments *)calloc(1, sizeof(struct arguments));
     args[t]->myTid = t;
     args[t]->buffer = &buffer;
     args[t]->buffer_size = &buffer_size;
     args[t]->num_updates_even = &num_updates_even;
     args[t]->num_updates_odd = &num_updates_odd;
     args[t]->lock = lock;
     args[t]->lock_even = lock_even;
     args[t]->lock_odd = lock_odd;
     args[t]->is_full = is_full;
     args[t]->is_empty = is_empty;

  }

  // Creating child threads
  for (t=0; t < num_threads; t++) {
    if (pthread_create(&worker_thread[t], NULL,
                      do_work, (void *)args[t])) {
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
    
  printf("\nNumber of odd updates: %d\n", (num_updates_odd));
  printf("Number of even updates: %d\n", (num_updates_even));
  printf("Total updates: %d \n", (num_updates_odd + num_updates_even));
  exit(0);

  printf("\n\n");
}

void *do_work(void *arg) {
    
  // initialize variables
  long int myTid;
  int *buffer;
  int *buffer_size;
  int *num_updates_even;
  int *num_updates_odd;

  pthread_mutex_t lock, lock_even, lock_odd;
  pthread_cond_t is_full, is_empty;

  // initialize local struct
  struct arguments *argument;
  argument = (struct arguments *)arg;

  myTid = argument->myTid;
  buffer = argument->buffer;
  buffer_size = argument->buffer_size;
  num_updates_even = argument->num_updates_even;
  num_updates_odd = argument->num_updates_odd;
  lock = argument->lock;
  lock_even = argument->lock_even;
  lock_odd = argument->lock_odd;
  is_full = argument->is_full;
  is_empty = argument->is_empty;

  while (*num_updates_odd != 10 || *num_updates_even != 10)
  {
     //Code for even threads
     //Add to buffer
     if (myTid%2==0) {
       pthread_mutex_lock(&lock);

       while (*buffer_size == 1 && *num_updates_even != 10) {
            fprintf(stderr,"\n[Tid %d] Waiting", myTid);
            if (*num_updates_even != 10) {
               pthread_cond_wait(&is_empty, &lock);
            }
       }


       *buffer=myTid;

       pthread_mutex_lock(&lock_even);
       if (*num_updates_even != 10) {

          fprintf(stderr,"\n[Tid %d] Buffer is: %d", myTid, *buffer);

          *buffer_size += 1;
          *num_updates_even += 1;

          printf("\neven updates %d\n", *num_updates_even);
       }
       pthread_mutex_unlock(&lock_even);

       pthread_cond_signal(&is_full);
       pthread_mutex_unlock(&lock);

       usleep(100000);
    }

    //Code for odd threads
    //Remove from buffer
    else if (myTid%2==1) {
       pthread_mutex_lock(&lock);
       while (*buffer_size == 0 && *num_updates_odd != 10) {

        fprintf(stderr,"\n[Tid %d] Waiting", myTid);

        if (*num_updates_odd != 10) {

           pthread_cond_wait(&is_full, &lock);
        }
     }

      int myBuffer = *buffer;


      pthread_mutex_lock(&lock_odd);
      if (*num_updates_odd != 10)
      {
         fprintf(stderr,"\n[Tid %d] Buffer is: %d", myTid, myBuffer);
         *buffer_size -= 1;
         *num_updates_odd += 1;
         printf("\nupdate odd %d", *num_updates_odd);
      }
      pthread_mutex_unlock(&lock_odd);

      pthread_cond_signal(&is_empty);
      pthread_mutex_unlock(&lock);

      usleep(100000);
    }
}
  return NULL;
}
