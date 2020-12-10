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
    int *flag;
    pthread_mutex_t mutex;
};

void *do_work1(void *arg);
void *do_work2(void *arg);

int main()
{
    const int NUM_THREADS = 6; // 6 threads, 3 per section
    
    pthread_t threads[NUM_THREADS];
    struct arguments *arguments[NUM_THREADS];
    pthread_mutex_t mutex1, mutex2;
    int buffer1[3], buffer2[3];
    int counter1[1] = {0};
    int counter2[1] = {0};
    int correct1[1] = {0};
    int correct2[1] = {0};
    int flag[1] = {0};
    
    pthread_mutex_init( &mutex1, NULL );
    pthread_mutex_init( &mutex2, NULL );
    for(int i = 0; i < NUM_THREADS/2; i++)
    {
        buffer1[i] = 0;
        buffer2[i] = 0;
    }
    
    for(int i = 0; i < NUM_THREADS; i++)
    {
        arguments[i] = (struct arguments *)calloc(1, sizeof(struct arguments));
        arguments[i]->id = i + 1;
        arguments[i]->flag = flag;
        if(i < 3)
        {
            arguments[i]->counter = counter1;
            arguments[i]->correct = correct1;
            arguments[i]->buffer = buffer1;
            arguments[i]->mutex = mutex1;
            pthread_create(&threads[i], NULL, do_work1, (void *)arguments[i]);
        }
        else
        {
            arguments[i]->counter = counter2;
            arguments[i]->correct = correct2;
            arguments[i]->buffer = buffer2;
            arguments[i]->mutex = mutex2;
            pthread_create(&threads[i], NULL, do_work2, (void *)arguments[i]);
        }
    }
    
    for(int i = 0; i < NUM_THREADS; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    printf("Total sequences generated team1: %d\n", counter1[0]);
    printf("Number of correct sequences team1: %d\n", correct1[0]);
    
    printf("Total sequences generated team2: %d\n", counter2[0]);
    printf("Number of correct sequences team2: %d\n", correct2[0]);
}

void *do_work1(void *arg)
{
    struct arguments *arg1 = (struct arguments *)arg;
    int id = arg1->id;
    int *counter = arg1->counter;
    int *correct = arg1->correct;
    int *buffer = arg1->buffer;
    int *flag = arg1->flag;
    pthread_mutex_t mutex = arg1->mutex;
    int index = 0;
    
    while(correct[0] < 10 && flag[0] == 0)
    {
        usleep(50000);
        pthread_mutex_lock(&mutex);
        
        if(correct[0] >= 10 || flag[0] > 0)
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
            if(buffer[index] == 0)
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
        usleep(50000);
    }
    if(flag[0] == 0)
    {
        printf("Team 1 won!\n");
        flag[0] = 1;
    }
}

void *do_work2(void * arg)
{
    struct arguments *arg2 = (struct arguments *)arg;
    int id = arg2->id;
    int *counter = arg2->counter;
    int *correct = arg2->correct;
    int *buffer = arg2->buffer;
    int *flag = arg2->flag;
    pthread_mutex_t mutex = arg2->mutex;
    int index = 0;
    
    while(correct[0] < 10 && flag[0] == 0)
    {
        usleep(50000);
        pthread_mutex_lock(&mutex);
        
        if(correct[0] >= 10 || flag[0] > 0)
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
            if(buffer[index] == 0)
            {
                printf("My id: %d\n", id);
                buffer[index] = id;
                break;
            }
        }
        
        if(index == 2)
        {
            counter[0] = counter[0] + 1;
            if(buffer[0] == 4 && buffer[1] == 5 && buffer[2] == 6)
            {
                printf("456\n");
                correct[0] = correct[0] + 1;
            }
            buffer[0] = 0;
            buffer[1] = 0;
            buffer[2] = 0;
        }
        
        pthread_mutex_unlock(&mutex);
        usleep(50000);
    }
    
    if(flag[0] == 0)
    {
        printf("Team 2 won!\n");
        flag[0] = 1;
    }
}
