#include <stdlib.h>
#include <omp.h>
#include <stdio.h>

void *do_work1(void *arg);
void *do_work2(void *arg);

int main(int argc, char *argv) {
    int array[100];
    int i,sum;
    pthread_t worker_thread1;
    pthread_t worker_thread2;
    struct arguments *arg;
    
    // Create thread1
    if (pthread_create(&worker_thread1, NULL,
    do_work1, (void *)arg)) {
        fprintf(stderr,"Error while creating thread\n");
        exit(1);
    }
    // Create thread2
    if (pthread_create(&worker_thread2, NULL,
    do_work2, (void *)arg)) {
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
    
    exit(0);
}

    void *do_work1(void *arg) {
        printf("\nStarting1");
        printf("\nExiting1");
        return NULL;
    }

    void *do_work2(void *arg) {
        printf("\nStarting2");
        printf("\nExiting2");
        return NULL;
    }
