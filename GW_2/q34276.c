#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>


struct arguments{
int tid;
int * scoreA;
int * scoreB;
int * passA;
int * passB;
int * prevID;
pthread_mutex_t * lock;
sem_t * semarray;
};


//Prototypes
int randThread(int team, int min, int max);
void *do_work(void *arg);


int main(int argc, char **argv) {

  //Seed rng
  //Do not change seed
  srand(42);

  //write code here
  const int COUNT = 10;
  int scoreA = 0;
  int scoreB = 0;
  int passA = 0;
  int passB = 0;
  int prevID = 0;
  int index;

  struct arguments *arg[COUNT];

  pthread_mutex_t lock;
  pthread_mutex_init(&lock, NULL);

  pthread_t worker_thread[COUNT];

  sem_t *semarray = calloc(COUNT, sizeof(sem_t));

  sem_init(&semarray[0], 0, 1);

  for(index = 1; index < COUNT; index++) {
    sem_init(&semarray[index], 0, 1);
  }

  //create threads with ids
  for(index = 0; index < COUNT; index++)
  {
      //creating ids
      arg[index] = (struct arguments *)calloc(1, sizeof(struct arguments));
      arg[index]->tid = index;
      arg[index]->scoreA = &scoreA;
      arg[index]->scoreB = &scoreB;
      arg[index]->passA = &passA;
      arg[index]->passB = &passB;
      arg[index]->prevID = &prevID;
      arg[index]->semarray = semarray;
      arg[index]->lock = &lock;

      //creating threads
      if(pthread_create(&worker_thread[index], NULL,
                                   do_work, (void *)arg[index]))
      {
          fprintf(stderr, "Error while creating thread\n");
          return 1;
      }
  }

  //join threads
  for(index = 0; index < COUNT; index++)
  {
      if(pthread_join(worker_thread[index], NULL))
      {
          fprintf(stderr, "Error while joining with child thread\n");
          return 1;
      }
  }

  //print output
  printf("final score: Team0 - %d, Team1 - %d\n", scoreA, scoreB);

  return 0;

  //free(args);
  printf("\n\n");
  exit(0);
}

//Do not modify
//Team must be 0 or 1
//0 for team 0
//1 for team 1
//min- minimum thread id to generate
//max- maximum thread id to generate
//Min= 0
//Max= 10
//Generated in [0, 10)
int randThread(int team, int min, int max)
{
    double randNum = (double)rand() / (double)RAND_MAX;
    //Generate random semaphore to send the ball/puck/etc. to
    //On the team
    if (randNum<0.6)
    {
      double randNum = ((double)rand() / (double)RAND_MAX)*(max*1.0/2*1.0);
      //send to teammate
      if (team==0)
      {
      return (int) randNum;
      }
      if (team==1)
      {
        return (int)randNum+5;
      }
    }
    //Interception by other team
    else
    {
      double randNum = 5.0*((double)rand() / (double)RAND_MAX);
      //send to other team
      if (team==1)
      {
      return (int) randNum;
      }
      if (team==0)
      {
      return (int)randNum+5;
      }
    }
}

void *do_work(void *arg)
{
    struct arguments *argument;
    argument = (struct arguments *)arg;

    //unpacking arguements
    int myid = argument->tid;
    int *passLow = argument->passA;
    int *passHigh = argument->passB;
    int *scoreLow = argument->scoreA;
    int *scoreHigh = argument->scoreB;
    int *prevID = argument->prevID;
    sem_t *semarray = argument->semarray;
    sem_t mysem = semarray[myid];
    pthread_mutex_t *lock = argument->lock;

    int random;

    while(*scoreLow < 10 && *scoreHigh < 10) {

      sem_wait(&mysem);

      pthread_mutex_lock(lock);

      if(myid < 5) {
        *passLow += 1;
        if(*prevID > 4) {
          printf("[Interception: Team: 0] Thread: %d, Intercepted by Thread: %d, Counter Team 1: 0\n",
                  *prevID, myid);
          *passLow = 0;
          *passHigh = 0;
        }
        else {
          printf("[Pass: Team: 0] Thread: %d, Pass to Thread: %d, Counter Team 0: %d\n", *prevID, myid, *passLow);
        }
        if(*passLow == 5) {
          *scoreLow += 1;
          printf("[Team 0 Scored A Net!] Score Team 0: %d, Score Team 1: %d, Counter Team 0: 5\n",
                  *scoreLow, *scoreHigh);
          *passLow = 0;
          printf("[Team 0] Counter reset: %d\n", *passLow);
        }

        if( *scoreLow == 10)
        {
            printf("\n[Team 0 WINS!!!!!]\n\n");
            exit(1);
        }
        else
        {
            random = randThread(0, 0, 10);
            sem_post(&semarray[random]);
        }

      }

      else {
        *passHigh += 1;
        if(*prevID < 5) {
          printf("[Interception: Team: 1] Thread: %d, Intercepted by Thread: %d, Counter Team 0: 0\n",
                  *prevID, myid);
            *passLow = 0;
            *passHigh = 0;
        }
        else {
          printf("[Pass: Team: 1] Thread: %d, Pass to Thread: %d, Counter Team 1: %d\n", *prevID, myid, *passHigh);
        }
        if(*passHigh == 5) {
        *scoreHigh += 1;
          printf("[Team 1 Scored A Net!] Score Team 0: %d, Score Team 1: %d, Counter Team 1: 5\n",
                  *scoreLow, *scoreHigh);
          *passHigh = 0;
          printf("[Team 1] Counter reset: %d\n", *passHigh);
        }
        if( *scoreHigh == 10)
        {
            printf("\n[Team 1 WINS!!!!!]\n\n");
            exit(1);
        }
        else
        {
            random = randThread(1, 0 ,10);
            sem_post(&semarray[random]);
        }

      }

      *prevID = myid;
      pthread_mutex_unlock(lock);

    }

    return NULL;

}
