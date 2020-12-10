#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

struct args
{
    int id;
    int *count;
    int *suc;
    int *buffer;
    pthread_mutex_t mutex;
};

void *do_work(void *arg);

int main()
{
    pthread_t threads[3];
    struct args *arguments[3];
    pthread_mutex_t mutex;
    int buffer[3];
    int count[1] = {0};
    int suc[1] = {0};
    
    pthread_mutex_init(&mutex, NULL);
    for(int i = 0; i < 3; i++)
    {
        buffer[i] = 0;
    }
    
    for(int i = 0; i < 3; i++)
    {
        arguments[i] = (struct args *)calloc(1, sizeof(struct args));
        arguments[i]->id = i + 1;
        arguments[i]->count = count;
        arguments[i]->suc = suc;
        arguments[i]->buffer = buffer;
        arguments[i]->mutex = mutex;
        pthread_create(&threads[i], NULL, do_work, (void *)arguments[i]);
    }
    
    for(int i = 0; i < 4; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    printf("Total sequences generated: %d\n", count[0]);
    printf("Number of correct sequences: %d\n", suc[0]);
}

void *do_work(void *arg)
{
    struct args *argu = (struct args *)arg;
    int id = argu->id;
    int *count = argu->count;
    int *suc = argu->suc;
    int *buffer = argu->buffer;
    pthread_mutex_t mutex = argu->mutex;
    int index = 0;
    
    while(suc[0] < 10)
    {
        pthread_mutex_lock(&mutex);
        
        if(suc[0] >= 10)
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
            count[0] = count[0] + 1;
            if(buffer[0] == 1 && buffer[1] == 2 && buffer[2] == 3)
            {
                printf("123\n");
                suc[0] = suc[0] + 1;
            }
            buffer[0] = 0;
            buffer[1] = 0;
            buffer[2] = 0;
        }
        
        pthread_mutex_unlock(&mutex);
        usleep(500000);
    }
}
