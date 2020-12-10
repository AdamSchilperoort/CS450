#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <unistd.h>
#include <semaphore.h>


struct arguments{
int * scoreA;
int * scoreB;
int * counterTeamA;
int * counterTeamB;
sem_t * sem_ls;
int tid;
int teamNum;
};


//Prototypes
int randThread(int team, int min, int max);
void *do_work(void *arg);



int main(int argc, char **argv) {

   const int NUMBEROFTHREADS = 10;

  sem_t *sem_ls = calloc(NUMBEROFTHREADS, sizeof(sem_t));

  for (int i = 0; i < NUMBEROFTHREADS; i++) {
    sem_init(&sem_ls[i], 0, 0);
  }

  sem_post(&sem_ls[0]);

  //Seed rng
  //Do not change seed
  srand(42);


  //write code here

  int index;
  int team0 = 0;
  int team1 = 1;
  int scoreA = 0;
  int scoreB = 0;
  int counterTeamA = 0;
  int counterTeamB = 0;
  pthread_t worker_thread[NUMBEROFTHREADS];
  struct arguments * args[NUMBEROFTHREADS];



  // initialize semaphores and struct
  for (index = 0; index < NUMBEROFTHREADS; index++) {
     args[index] = (struct arguments *)calloc(1, sizeof(struct arguments));
     args[index]->tid = index;
     args[index]->scoreA = &scoreA;
     args[index]->scoreB = &scoreB;
     args[index]->counterTeamA = &counterTeamA;
     args[index]->counterTeamB = &counterTeamB;
     args[index]->semarray = semarray;
  }

  // create threads
  for (index = 0; index < NUMBEROFTHREADS; index++) {
    if(pthread_create( &worker_thread[index], NULL, do_work,
      (void *)args[index]))
    {
      fprintf(stderr, "error while create thread\n");
      exit(1);
    }
  }
  // joining all the threads together
  for (index = 0; index < NUMBEROFTHREADS; index++) {

    if (pthread_join(worker_thread[index], NULL))
    {
      fprintf(stderr, "Error while joining\n");
      exit(1);
    }
  }

  // freememory
  for (index = 0; index < NUMBEROFTHREADS; index++)
  {
    free(args[index]);
  }

  printf("\n\n");
  exit(0);
}

void *do_work(void *arg) {

   // initalize varibles
  int tid;
  int index = 0;
  int teamNum;
  int nextid;
  int *scoreA;
  int *scoreB;
  int *counterTeamA;
  int *counterTeamB;
  sem_t *sem_ls;
  struct arguments *argument;

  argument = (struct arguments *)arg;

  // assign arguments to local varibles
  tid = argument->tid;
  teamNum = argument->teamNum;
  scoreA = argument->scoreA;
  scoreB = argument->scoreB;
  counterTeamA = argument->counterTeamA;
  counterTeamB = argument->counterTeamB;
  sem_ls = argument->sem_ls;


  // while score isnt ten
  while (*scoreA < 10 && *scoreB < 10) {

    sem_wait(&sem_ls[tid]);

    nextid=randThread(teamNum, 0, 10);

    if (*scoreA < 10 && *scoreB < 10) {

      // Team 0
      if (tid <= 4) {

        if (nextid <= 4) {

          (*counterTeamA)++;

          printf("[Pass: Team: 0] Thread: %d, Pass to Thread: %d, "
           "Counter Team 0: %d\n", tid, nextid, *counterTeamA);

          if (*counterTeamA == 5) {
            (*scoreA)++;
            printf("[Team 0 Scored A Net!] Score Team 0: %d, Score Team 1: %d, "
            "Counter Team 0: 5\n", *scoreA, *scoreB);

            *counterTeamA = 0;
            printf("[Team 0] Counter Reset: 0\n");
          }
        }

        else {
          (*counterTeamA) = 0;

          printf("[Interception: Team: 0] Thread: %d, "
          "Intercepted by Thread: %d, Counter Team 0: %d\n",
           tid, nextid, *counterTeamA);
        }
      }

      // Team 1
      else if (tid <= 9) {

        if (nextid >= 5) {

          (*counterTeamB)++;

          printf("[Pass: Team: 1] Thread: %d, Pass to Thread: %d, "
          "Counter Team 1: %d\n", tid, nextid, *counterTeamB);

          if (*counterTeamB == 5) {
            (*scoreB)++;
            printf("[Team 1 Scored A Net!] Score Team 0: %d, Score Team 1: %d, "
            "Counter Team 0: 5\n", *scoreA, *scoreB);

            *counterTeamB = 0;
            printf("[Team 1] Counter Reset: 0\n");
          }
        }

        else {

          (*counterTeamB) = 0;

          printf("[Interception: Team: 1] Thread: %d, Intercepted by Thread: "
          "%d, Counter Team 1: %d\n", tid, nextid, *counterTeamB);
        }

      }
   }

     sem_post(&sem_ls[nextid]);
  }

  // post all remaining waiting semaphores so program can end
  for (index = 0; index < 10; index++) {
     sem_post(&sem_ls[index]);
  }

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
