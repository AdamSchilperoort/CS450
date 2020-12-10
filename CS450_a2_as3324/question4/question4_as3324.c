#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

struct arg
{
    int *counter;
    pthread_mutex_t mutex;
    pthread_cond_t *empty;
    pthread_cond_t *full;
};

//pthread_mutex_t mutex;
//pthread_cond_t full, empty;

void *increment_work(void *args);
void *decrement_work(void *args);

int main()
{
    int count[] = {0};
    pthread_t thread1, thread2;
    struct arg *arg1, *arg2;
    pthread_mutex_t mutex;
    pthread_cond_t full, empty;
    
    pthread_mutex_init(&mutex, NULL );
    pthread_cond_init(&empty, NULL);
    pthread_cond_init(&full, NULL);
    
    arg1 = (struct arg *)calloc(1, sizeof(struct arg));
    arg1->counter = count;
    arg1->mutex = mutex;
    arg1->empty = &empty;
    arg1->full = &full;
    
    arg2 = (struct arg *)calloc(1, sizeof(struct arg));
    arg2->counter = count;
    arg2->mutex = mutex;
    arg2->empty = &empty;
    arg2->full = &full;
    
    pthread_create(&thread1, NULL, increment_work, (void *)arg1);
    pthread_create(&thread2, NULL, decrement_work, (void *)arg2);
    
    pthread_join(thread1, NULL);
    pthread_join(thread2, NULL);
    
    return 0;
}

void *increment_work(void *args)
{
    struct arg *argu = (struct arg *)args;
    int *count = argu->counter;
    pthread_mutex_t mutex = argu->mutex;
    pthread_cond_t *empty = argu->empty;
    pthread_cond_t *full = argu->full;
    
    for(int i = 0; i < 3; i++)
    {
        pthread_mutex_lock(&mutex);
        
        while(count[0] > 0)
        {
            pthread_cond_wait(full, &mutex);
        }
        
        for(int i = 0; i <= 9; i++)
        {
            count[0] = count[0] + 1;
            printf("Count is now (inc fn): %d\n", count[0]);
            usleep(500000);
        }
        
        pthread_cond_signal(empty);
        pthread_mutex_unlock(&mutex);
    }
}

void * decrement_work(void *args)
{
    struct arg *argu = (struct arg *)args;
    int *count = argu->counter;
    pthread_mutex_t mutex = argu->mutex;
    pthread_cond_t *empty = argu->empty;
    pthread_cond_t *full = argu->full;
    
    for(int i = 0; i < 2; i++)
    {
        pthread_mutex_lock(&mutex);
        
        while(count[0] <= 1)
        {
            pthread_cond_wait(empty, &mutex);
        }
        
        for(int i = 0; i <= 9; i++)
        {
            count[0] = count[0] - 1;
            printf("Count is now (dec fn): %d\n", count[0]);
            usleep(500000);
        }
        
        pthread_cond_signal(full);
        pthread_mutex_unlock(&mutex);
    }
}
