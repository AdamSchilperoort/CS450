#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>

int total = 0;

struct arg
{
    int id;
    int *nextNum;
    pthread_mutex_t mutex;
    pthread_cond_t *flags;
};

void *do_work(void *args);

int main()
{
    pthread_t threads[10];
    struct arg *arguments[10];
    int nextNum[] = {0};
    pthread_mutex_t mutex;
    pthread_cond_t flags[10];
    
    pthread_mutex_init( &mutex, NULL );
    for(int i = 0; i < 10; i++)
    {
    	pthread_cond_init(&flags[i], NULL);
    }
    
    for(int i = 0; i < 10; i++)
    {
        arguments[i] = (struct arg *)calloc(1, sizeof(struct arg));
        arguments[i]->id = i;
        arguments[i]->nextNum = nextNum;
        arguments[i]->mutex = mutex;
        arguments[i]->flags = flags;
        pthread_create(&threads[i], NULL, do_work, (void *)arguments[i]);
    }
    
    for(int i = 0; i < 10; i++)
    {
        pthread_join(threads[i], NULL);
    }
    
    printf("Total: %d\n", total);
}

void *do_work(void *args)
{
    struct arg *argu = (struct arg *)args;
    int id = argu->id;
    int *nextNum = argu->nextNum;
    pthread_mutex_t mutex = argu->mutex;
    pthread_cond_t *flags = argu->flags;
    
    for(int i = 0; i < 22; i++)
    {
        pthread_mutex_lock(&mutex);
        
        while(nextNum[0] != id)
        {
            pthread_cond_wait(&flags[id], &mutex);
        }
        
	    nextNum[0] = (id + 1) % 10;
	    total += id;
        printf("My id: %d, total: %d\n", id, total);
            
        pthread_cond_signal(&flags[nextNum[0]]);
        pthread_mutex_unlock(&mutex);
    }
}
