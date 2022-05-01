#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>  // fork()
#include <signal.h>  // signal()
#include <pthread.h> // pthread_create, pthread_join, etc.

#include <sys/wait.h>
#include <sys/sem.h>
#include "socket_utils.h"
#define BUFSIZE 1024
#define NUM_THREADS 8
#define SEM_KEY 12345678
#define SEM_MODE 0666

int total_amount = 0;
void connectCallback(int conn_fd);

int sem;
int P(int s);
int V(int s);

int sock_fd;
int stop=0;
void intHandler(int signum);

int main(int argc, char **argv)
{
  int conn_fd;
  struct sockaddr_in cln_addr;
  socklen_t sLen = sizeof(cln_addr);
  pthread_t thread;

  if (argc != 2)
  {
    printf("Usage: %s <port>\n", argv[0]);
    exit(-1);
  }

  sock_fd = createServerSock(atoi(argv[1]), TRANSPORT_TYPE_TCP);
  if (sock_fd < 0)
  {
    perror("Error create socket\n");
    exit(-1);
  }

  // Creating semaphore
  sem = semget(SEM_KEY, 1, IPC_CREAT | IPC_EXCL | SEM_MODE);
  // sem = semget(SEM_KEY, 1, IPC_CREAT | SEM_MODE);
  if (sem < 0){
    perror("Error create semaphore\n");
    exit(-1);
  }
  printf("sem created\n");
  if( semctl(sem, 0, SETVAL, 1) < 0){
    perror("Error semctl\n");
    exit(-1);
  }

  signal(SIGINT, intHandler);

  while (!stop)
  {
    conn_fd = accept(sock_fd, (struct sockaddr *)&cln_addr, &sLen);
    if (conn_fd == -1)
    {
      perror("Error: accept()");
      continue;
    }
    if (pthread_create(&thread, NULL, (void *(*)(void *)) & connectCallback, (void *)conn_fd))
    {
      perror("Error: pthread_create()\n");
    }
    // connectCallback(conn_fd, &total_amount);
  }
  printf("closing sock_fd\n");
  close(sock_fd);
  return 0;
}

void connectCallback(int conn_fd)
{
  char rcv[BUFSIZ];
  int n;
  
  while ((n = read(conn_fd, rcv, BUFSIZE)) != 0)
  {
    // printf("%.*s\n", n, rcv); // print string with given length
    P(sem);
    if (rcv[0] == 'D')
    {
      // printf("+%d\n", atoi(rcv + 2));
      total_amount += atoi(rcv + 2);
      printf("after deposit: %d\n", total_amount);
    }
    else if (rcv[0] == 'W')
    {
      // printf("-%d\n", atoi(rcv + 2));
      total_amount -= atoi(rcv + 2);
      printf("after withdraw: %d\n", total_amount);
    }
    V(sem);
    // printf("size = %d\n", n);
    // printf("balance = %d\n", total_amount);
  }

  // printf("exit\n");
  close(conn_fd);
  pthread_exit(NULL);
  return;
}

int P(int s){
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = -1;
  sop.sem_flg = 0;

  if(semop(s, &sop, 1) < 0){
    perror("P(): sem failed\n");
    return -1;
  }else{
    return 0;
  }
}

int V(int s){
  struct sembuf sop;
  sop.sem_num = 0;
  sop.sem_op = 1;
  sop.sem_flg = 0;

  if(semop(s, &sop, 1) < 0){
    perror("V(): sem failed\n");
    return -1;
  }else{
    return 0;
  }
}

void intHandler(int signum)
{

  stop = 1;
  printf("closing sock_fd\n");
  close(sock_fd);

  printf("remove sem\n");
  if(semctl(sem, 0, IPC_RMID, 0) < 0){
    perror("Error removing sem\n");
    exit(-1);
  }
}
