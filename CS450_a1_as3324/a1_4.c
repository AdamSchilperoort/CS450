
#include <stdlib.h>
#include <omp.h>
#include <stdio.h>

struct arguments {
    pthread_mutex_t *mutex;
    int val;
    int *count;
};

void *do_work(void *arg);

int main(int argc, char *argv) {
    pthread_t worker_thread1;
    pthread_t worker_thread2;
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);

    struct arguments *arg[2];
    int count=0;
    
    // Build argument to threads
    arg[0] = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg[0]->mutex=&lock;
    arg[0]->val=2;
    arg[0]->count=&count;
    arg[1] = (struct arguments *)calloc(1, sizeof(struct arguments));
    arg[1]->mutex=&lock;
    arg[1]->val=3;
    arg[1]->count=&count;
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
    
    exit(0);
}

    void *do_work(void *arg) {
        struct arguments *argument;
        argument=(struct arguments*)arg;
        
        pthread_mutex_t *mutex=argument->mutex;
        int val=argument->val;
        int *count=argument->count;
        
        pthread_mutex_lock(mutex);
        *count+=val;
        printf("\n%d",*count);
        pthread_mutex_unlock(mutex);
        
        return NULL;
    }
