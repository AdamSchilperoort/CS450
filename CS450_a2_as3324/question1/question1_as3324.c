#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

struct arguments
{
    int id;
    int *counter;
    int *correct;
    int *buffer;
    pthread_mutex_t mutex;
};

void *do_work(void *arg);

int main()
{
    const int NUM_THREADS = 3;
    int index;
    
    int buffer[NUM_THREADS];
    int counter[1] = {0};
    int correct[1] = {0};
    
    pthread_mutex_t lock;
    pthread_mutex_init(&lock, NULL);
    
    pthread_t worker_thread[NUM_THREADS];
    struct arguments *arg[NUM_THREADS];
    
    for(index = 0; index < NUM_THREADS; index++)
    {
        buffer[index] = 0;
    }
    
    // create threads with IDs
    for(index = 0; index < NUM_THREADS; index++)
    {
        //creating IDs
        arg[index] = (struct arguments *)calloc(1, sizeof(struct arguments));
        arg[index]->id = index + 1;
        arg[index]->counter = counter;
        arg[index]->correct = correct;
        arg[index]->buffer = buffer;
        arg[index]->mutex = lock;
        
        //creating threads
        if(pthread_create(&worker_thread[index], NULL, do_work, (void *)arg[index]))
        {
            fprintf(stderr, "Error while creating thread\n");
            return 1;
        }
    }
    
    // join threads
    for(index = 0; index < NUM_THREADS; index++)
    {
         if(pthread_join(worker_thread[index], NULL))
        {
            fprintf(stderr, "Error while joining with child thread\n");
            return 1;
        }
    }
    
    // output
    printf("Total sequences generated: %d\n", counter[0]);
    printf("Number of correct sequences: %d\n", correct[0]);
    
    return 0;
}

void *do_work(void *arg)
{
    struct arguments *argument;
    argument = (struct arguments *)arg;
    
    //unpacking arguments
    int id = argument->id;
    int *counter = argument->counter;
    int *correct = argument->correct;
    int *buffer = argument->buffer;
    pthread_mutex_t mutex = argument->mutex;
    
    int index = 0;
    
    while(correct[0] < 10)
    {
        pthread_mutex_lock(&mutex);
        
        if(correct[0] >= 10)
        {
            pthread_mutex_unlock(&mutex);
            break;
        }
        
        for(index = 0; index < 3; index++)
        {
            if(buffer[index] == id)
            {
                index = -1;
                break;
            }
            else if(buffer[index] == 0)
            {
                printf("My id: %d\n", id);
                buffer[index] = id;
                break;
            }
        }
        
        if(index == 2)
        {
            counter[0] = counter[0] + 1;
            if(buffer[0] == 1 && buffer[1] == 2 && buffer[2] == 3)
            {
                printf("123\n");
                correct[0] = correct[0] + 1;
            }
            buffer[0] = 0;
            buffer[1] = 0;
            buffer[2] = 0;
        }
        
        pthread_mutex_unlock(&mutex);
        usleep(500000);
    }
}


