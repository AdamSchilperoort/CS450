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
  int counter = 0;
  int passes = 0;
  int scoreA = 0;
  int scoreB = 0;
  int passA = 0;
  int passB = 0;
  int index;

  struct arguments * args[COUNT];

  pthread_mutex_t lock;
  pthread_mutex_init(&lock, NULL);

  pthread_t worker_thread[COUNT];
  struct arguments *arg[COUNT];

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
      arg[index]->semarray = semarray;
      arg[index]->lock = &lock;

      //creating threads
      if(pthread_create(&worker_thread[index], NULL,
                                   do_work, (void *)arg[index]))
      {
          printf(stderr, "Error while creating thread\n");
          exit(1);
      }
  }

  //join threads
  for(index = 0; index < COUNT; index++)
  {
      if(pthread_join(worker_thread[index], NULL))
      {
          printf(stderr, "Error while joining with child thread\n");
          exit(1);
      }
  }

  //print output
  printf("final score: Team0 - %d, Team1 - %d\n", scoreA, scoreB);

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
    sem_t *semarray = argument->semarray;
    sem_t mysem = semarray[myid];
    pthread_mutex_t *lock = argument->lock;

    int random;

    while(*scoreLow < 10 && *scoreHigh < 10) {

      sem_wait(&mysem);

      pthread_mutex_lock(lock);

      if(myid < 5) {
        *passLow += 1;
        if(*passHigh > 0) {
          printf("Interception!");
        }
        *passHigh = 0;
        if(*passLow == 5) {
          *scoreLow += 1;
          *passLow = 0;
        }

        random = randThread(0, 0, 10);
        sem_post(&semarray[random]);

      }

      else {
        *passHigh += 1;
        if( passLow > 0 )
        {
          printf("\n[Interception: Team: 1] Thread: %d", myid);
        }
        else
        {
          printf("\n[Pass: Team: 1] Thread: %d, to Thread: %d, Counter Team %d:",
                  previd, myid, passLow);
        }
        *passLow = 0;
        if(passHigh == 5) {
          printf("\n[Team 1 Scored A net!]");
          scoreHigh += 1;
          printf("Score Team 0: %d, Score Team 1: %d, Counter Team 1: %d",
                  scoreLow, scoreHigh, passHigh );
          passHigh = 0;
          printf("[Team 1] Counter Reset: %d", passHigh];
        }

        random = randThread(1, 0 ,10);
        sem_post(&semarray[random]);

      }

      pthread_mutex_unlock(lock);

    }

    return NULL;

}
