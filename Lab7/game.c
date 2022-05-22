#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h> // memset
#include "memory_util.h"

#include <signal.h>  // sigaction
#include <time.h>    // timespec, nanosleep
#include <sys/shm.h> // shmget, shmat, shmdt, shmctl

int answer, stop = 0;
guess_data *dataPtr;

void sigHandler(int);
void intHandler(int);

int main(int argc, char *argv[])
{
  int ret, shmId;
  key_t key;
  struct sigaction sa;
  struct timespec timer;

  if (argc != 3)
  {
    printf("Usage: %s <shm key> <answer>\n", argv[0]);
    exit(1);
  }

  key = atoi(argv[1]);
  answer = atoi(argv[2]);

  // Create shared memory
  if ((shmId = shmget(key, sizeof(guess_data), IPC_CREAT | 0666)) < 0)
  {
    perror("Error: shmget\n");
    exit(1);
  }
  if ((dataPtr = shmat(shmId, NULL, 0)) == (guess_data *)-1)
  {
    perror("Error: shmat\n");
    exit(1);
  }
  sprintf(dataPtr->result, "init");

  // Set timer
  memset(&timer, 0, sizeof(timer));
  timer.tv_nsec = 0;
  timer.tv_sec = 20;

  // Register signal
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = sigHandler;

  sigaction(SIGUSR1, &sa, NULL);
  signal(SIGINT, intHandler);

  printf("PID = %d\n", getpid());
  printf("Catching SIGUSR1...\n");
//   printf("answer = %d\n", answer);

  do
  {
    ret = nanosleep(&timer, &timer);
  } while (ret & !stop);

  // Detach and delete shared memory
  shmdt(dataPtr);
  if(shmctl(shmId, IPC_RMID, NULL) < 0){
    perror("Error: shmctl\n");
    exit(1);
  }

  printf("Done\n");
  return 0;
}

void sigHandler(int signum)
{
  // printf("Caught signal: %d\n", signum);

  if(answer > dataPtr->guess){
    sprintf(dataPtr->result, "larger");
  }else if(answer < dataPtr->guess){
    sprintf(dataPtr->result, "smaller");
  }else{ // answer == dataPtr->guess
    sprintf(dataPtr->result, "bingo");
    stop = 1;
  }

  printf("guess %d, ", dataPtr->guess);
  printf("%s\n", dataPtr->result);

}

void intHandler(int signum){
  stop = 1;
}