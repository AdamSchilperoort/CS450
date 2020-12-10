
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>

struct arguments {
    int *sum;
    int value;
};

void *do_work(void *arg);

int main(int argc, char *argv) {
    pthread_t worker_thread1;
    pthread_t worker_thread2;
    struct arguments *arg[2];
    
    int sum=0;
    
    // Build argument to threads
    arg[0] = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg[0]->value=3;
    arg[0]->sum=&sum;
    arg[1] = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg[1]->value=5;
    arg[1]->sum=&sum;
    
    // Create thread1
    if (pthread_create(&worker_thread1, NULL,
                       do_work, (void *)arg[0])) {
        fprintf(stderr,"Error while creating thread\n");
        exit(1);
    }
    
    // Create thread2
    if (pthread_create(&worker_thread2, NULL,
                       do_work, (void *)arg[1])) {
        fprintf(stderr,"Error while creating thread\n");
        exit(1);
    }
    
    // Join with thread
    if (pthread_join(worker_thread1, NULL)) {
        fprintf(stderr,"Error while joining with child thread\n");
        exit(1);
    }
    
    if (pthread_join(worker_thread2, NULL)) {
        fprintf(stderr,"Error while joining with child thread\n");
        exit(1);
    }
    printf("\nSum: %d",sum);
    exit(0);
}

void *do_work(void *arg) {
    struct arguments *argument;
    argument=(struct arguments*)arg;
    int val=argument->value;
    int *sum=argument->sum;
    *sum+=val;
    return NULL;
}
