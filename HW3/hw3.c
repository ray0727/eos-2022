#include<arpa/inet.h>
#include<errno.h>
#include<netdb.h>
#include<netinet/in.h>
#include<signal.h> 
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<sys/socket.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<fcntl.h>
#include<unistd.h> 
#include <ctype.h>
#include<stdbool.h>
#include <sys/sem.h>
#include<time.h>
#include <semaphore.h>
#include <pthread.h>
#include <sys/mman.h>
#include "socket_utils.h"
#define BUFSIZE 1024
#define SEM_MODE 0666
long int *glob_var;

void Handler(int signum);

struct restaurant{
    char meal1[100], meal2[100];
    int distance;
    int price1;
    int price2;
};

struct restaurant dessert, beverage, diner;
volatile int stop = 0;
char result[BUFSIZE], meal_first[BUFSIZE], meal_second[BUFSIZE];
int meal_type=0, first_meal_type, state_order=0, first_order, total_num[6]={0,0,0,0,0,0};
bool first = true;
void pick_meal(char *, char *, int);
void reset();

int main(int argc, char* argv[])
{
    char *filename = "result.txt";
    int sockfd, connfd, yes = 1;
    char rcv[BUFSIZE];
    char *p, *order[3];
    char delim[] = " ";
    int n, i=0, j=0;
    int pay=0, total_meals=0;
    
    memset(result,'\0',sizeof(result));
    memset(meal_first,'\0',sizeof(meal_first));
    memset(meal_second,'\0',sizeof(meal_second));
    strcpy(dessert.meal1, "cookie");
    strcpy(dessert.meal2, "cake");
    dessert.price1 = 60;
    dessert.price2 = 80;
    dessert.distance = 3;
    strcpy(beverage.meal1, "tea");
    strcpy(beverage.meal2, "boba");
    beverage.price1 = 40;
    beverage.price2 = 70;
    beverage.distance = 5;
    strcpy(diner.meal1, "fried-rice");
    strcpy(diner.meal2, "Egg-drop-soup");
    diner.price1 = 120;
    diner.price2 = 50;
    diner.distance = 8;
    //time part 
    time_t seconds;
    
    //socket
    struct sockaddr_in cln;
    socklen_t cln_len = sizeof(cln);

    pid_t child_pid;

    if(argc != 2)
    {
        printf("Usage:_%s_port\n", argv[0]);
        exit(-1);
    }
    
    sockfd = createServerSock(atoi(argv[1]), TRANSPORT_TYPE_TCP);
    if(sockfd < 0)
    {
        printf("Can't_create_socket:\n");
        exit(-1);
    }

    signal(SIGINT, Handler);
    //semaphore part
    sem_t *sema = mmap(NULL, sizeof(*sema), 
      PROT_READ |PROT_WRITE,MAP_SHARED|MAP_ANONYMOUS,
      -1, 0);
    if(sema == MAP_FAILED){
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    /* create/initialize semaphore */
    if(sem_init(sema, 1, 2) < 0){
        perror("sem_init");
        exit(EXIT_FAILURE);
    }


    //shared memory for record process
    glob_var = mmap(NULL, 4*sizeof (*glob_var), PROT_READ | PROT_WRITE, 
                    MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    glob_var[0] = 0;
    glob_var[1] = 0;
    glob_var[2] = 0;
    glob_var[3] = 0;


    //mutex part 
    pthread_mutex_t mutex;
    pthread_mutex_init(&mutex, NULL);
    
    // int process_idx = -1;
    while(!stop)
    {
        // process_idx += 1;
        connfd = accept(sockfd, ( struct sockaddr *) &cln, &cln_len);
        child_pid = fork();
        if(child_pid >= 0)
        {
            if(child_pid == 0)
            {
                
                while(1)
                {
                    if(connfd == -1)
                    {
                        printf("Error:_accept()\n");
                        exit(-1);
                    }
                    memset(rcv,'\0',sizeof(rcv));
                    while(1){
                        if((n = read(connfd, rcv, BUFSIZE)) == -1){
                            printf("Error: read()\n");
                            exit(-1);
                        }
                        if(strcmp(rcv,"")!=0){
                            break;
                        }
                    }
                    printf("rcv: %s\n", rcv);
                    p = strtok(rcv, delim);
                    while(p!=NULL){
                        order[i++] = p;
                        p = strtok(NULL, delim);  
                    }
                    i=0;
                    if(strcmp(order[0], "shop")==0){
                        memset(result,'\0',sizeof(result));
                        n = sprintf(result, "Dessert Shop:3km\n- cookie:60$|cake:80$\n\
Beverage Shop:5km\n- tea:40$|boba:70$\n\
Diner:8km\n- fried-rice:120$|Egg-drop-soup:50$%c", '\0');
                        if((n = write(connfd, result, n)) == -1){
                            printf("Error: write()\n");
                            exit(-1);
                        }
                    }
                    else if(strcmp(order[0], "order")==0){
                        pick_meal(order[1], order[2], connfd);
                    }
                    else if(strcmp(order[0], "cancel")==0)
                    {
                        reset();
                        pay = 0;
                        total_meals = 0;
                        break;
                    }
                    else if(strcmp(order[0], "confirm")==0)
                    {
                        for(j = 0; j< 6 ;j++)
                        {
                            total_meals+=total_num[j];
                        }
                        if(total_meals == 0)
                        {
                            memset(result,'\0',sizeof(result));
                            n = sprintf(result, "Please order some meals%c", '\0');
                            if((n = write(connfd, result, n)) == -1){
                                printf("Error: write()\n");
                                exit(-1);
                            }
                        }
                        else
                        {
                            //////////////////
                            //wait time part
                            int wait = 0;
                            int start_time = 0;
                            int sema_idx = 0;
                            seconds = time(NULL);
                            //////////////////////////////
                            //critical part for mutex
                            pthread_mutex_lock(&mutex);
                            // printf("[process %d] start time: %ld\n", getpid(), seconds);
                            //set glob_var for first time
                            if(glob_var[0] == 0)
                            {
                                glob_var[0] = seconds;
                                sema_idx = 0;
                            }
                            else if(glob_var[1] == 0)
                            {
                                glob_var[1] = seconds;
                                sema_idx = 1;
                            }
                            else if(glob_var[0] <= glob_var[1])
                            {
                                wait = glob_var[0] - seconds;
                                sema_idx = 0;
                            }
                            else
                            {
                                wait = glob_var[1] - seconds;
                                sema_idx = 1;
                            }
                            start_time = glob_var[sema_idx];
                            //order's arrive time for guests
                            if(first_order == 1)
                            {
                                glob_var[sema_idx] += 3;
                                wait += 3;
                                pay = total_num[0]*dessert.price1 +total_num[1]*dessert.price2;
                            }
                            else if(first_order == 2)
                            {
                                glob_var[sema_idx] += 5;
                                wait += 5;
                                pay = total_num[2]*beverage.price1 +total_num[3]*beverage.price2;
                            }
                            else if(first_order == 3)
                            {
                                glob_var[sema_idx] += 8;
                                wait += 8;
                                pay = total_num[4]*diner.price1 +total_num[5]*diner.price2;
                            }
                            
                            
                            if(wait > 30)
                            {
                                memset(result,'\0',sizeof(result));
                                n = sprintf(result, "Your delivery will take a long time, do you want to wait?%c", '\0');
                                if((n = write(connfd, result, n)) == -1){
                                    printf("Error: write()\n");
                                    exit(-1);
                                }
                                memset(rcv,'\0',sizeof(rcv));
                                while(1){
                                    if((n = read(connfd, rcv, BUFSIZE)) == -1){
                                        printf("Error: read()\n");
                                        exit(-1);
                                    }
                                    if(strcmp(rcv,"")!=0){
                                        break;
                                    }
                                }
                                if(strcmp(rcv, "No")==0)
                                {
                                    if(first_order == 1)
                                    {
                                        glob_var[sema_idx] -= 3;
                                    }
                                    else if(first_order == 2)
                                    {
                                        glob_var[sema_idx] -= 5;
                                    }
                                    else if(first_order == 3)
                                    {
                                        glob_var[sema_idx] -= 8;
                                    }
                                    
                                    pthread_mutex_unlock(&mutex);
                                    // printf("\n////////////\n[process %d:%d] say No!! \n////////////////\n\n", getpid(), process_idx);
                                    break;
                                }
                                memset(result,'\0',sizeof(result));
                            }
                            pthread_mutex_unlock(&mutex);
                            //end of critical part for mutex
                        
                            //end of wait time part
                            /////////////////////

                            while(1)
                            {
                                seconds = time(NULL);
                                if(seconds >= start_time)
                                {
                                    break;
                                }
                            }
                            sem_wait(sema);
                            // printf("[process %d:%d] entered the critical part\n", getpid(), process_idx);

                            // seconds = time(NULL);
                            // printf("time: %ld\n", seconds);
                            // *glob_var = seconds;
                            if(wait < 30)
                            {
                                memset(result,'\0',sizeof(result));
                                n = sprintf(result, "Please wait a few minutes...%c", '\0');
                                if((n = write(connfd, result, n)) == -1){
                                    printf("Error: write()\n");
                                    exit(-1);
                                }
                            }
                            if(first_order == 1)
                            {
                                sleep(3);
                            }
                            else if(first_order == 2)
                            {
                                sleep(5);
                            }
                            else if(first_order == 3)
                            {
                                sleep(8);
                            }
                            memset(result,'\0',sizeof(result));
                            n = sprintf(result, "Delivery has arrived and you need to pay %d$%c", pay,'\0');
                            if((n = write(connfd, result, n)) == -1){
                                printf("Error: write()\n");
                                exit(-1);
                            }
                            first_order = 0;
                            total_meals=0;
                            reset();
                            seconds = time(NULL);
                            // printf("[process %d] end time: %ld\n", getpid(), seconds);
                            sem_post(sema);
                            
                            pthread_mutex_lock(&mutex);
                            //open txt file part
                            FILE *fp = fopen(filename, "w");
                            glob_var[2] ++;
                            glob_var[3] += pay;
                            memset(result,'\0',sizeof(result));
                            n = sprintf(result, "customer: %ld, income: %ld\n", glob_var[2], glob_var[3]);
                            fputs(result,fp);
                            fclose(fp);
                            pthread_mutex_unlock(&mutex);
                            printf("confirm: %d\n", pay);
                            // printf("\n////////////\n[process %d:%d] left the critical part\n////////////////\n\n", getpid(), process_idx);
                            break;
                        }
                    }
                    
                }
                close(connfd);
                exit(0);
            }
            
        }
    }
    close(connfd);
    close(sockfd);
    
    return 0;
}

void pick_meal(char *meal, char *num, int connfd)
{
    int n;
    if(strcmp(meal, dessert.meal1)==0 || strcmp(meal, dessert.meal2)==0){
        state_order = 1;
        if(strcmp(meal, dessert.meal1)==0){
            meal_type = 0;
            total_num[meal_type] += atoi(num);
        }
        else{
            meal_type = 1;
            total_num[meal_type] += atoi(num);
        }
        if(first){
            first_order = 1;
            first_meal_type = meal_type;
            first = false;
        }
    }
    else if(strcmp(meal, beverage.meal1)==0 || strcmp(meal, beverage.meal2)==0){
        state_order = 2;
        if(strcmp(meal, beverage.meal1)==0){
            meal_type = 2;
            total_num[meal_type] += atoi(num);
        }
        else{
            meal_type = 3;
            total_num[meal_type] += atoi(num);
        }
        if(first){
            first_order = 2;
            first_meal_type = meal_type;
            first = false;
        }
    }
    else if(strcmp(meal, diner.meal1)==0 || strcmp(meal, diner.meal2)==0){
        state_order = 3;
        if(strcmp(meal, diner.meal1)==0){
            meal_type = 4;
            total_num[meal_type] += atoi(num);
        }
        else{
            meal_type = 5;
            total_num[meal_type] += atoi(num);
        }
        if(first){
            first_order = 3;
            first_meal_type = meal_type;
            first = false;
        }
    }
    if(state_order != first_order){
        // printf("Wrong Resturant\n");
        n = strlen(result);
    }
    else if(meal_type == first_meal_type){
        // printf("Same meal\n");
        n = sprintf(meal_first, "%s %d%c", meal, total_num[meal_type],'\0');
    }
    else{
        // printf("Same Resturant, different meal\n");
        n = sprintf(meal_second, "%s %d%c", meal, total_num[meal_type],'\0');
    }
    if((first_meal_type%2)==0){
        if(strlen(meal_second)==0){
            n = sprintf(result, "%s%c", meal_first, '\0');
        }
        else{
            n = sprintf(result, "%s|%s%c", meal_first, meal_second, '\0');
        }
    }
    else{
        if(strlen(meal_second)==0){
            n = sprintf(result, "%s%c", meal_first, '\0');
        }
        else{
            n = sprintf(result, "%s|%s%c", meal_second, meal_first, '\0');
        }
    }
    printf("Ans: %s\n", result);
    if((n = write(connfd, result, n)) == -1){
        printf("Error: write()\n");
        exit(-1);
    }
}

void reset(){
    int i;
    first = true;
    for(i=0; i<6; i++){
        total_num[i] = 0;
    }
    memset(result,'\0',sizeof(result));
    memset(meal_first,'\0',sizeof(meal_first));
    memset(meal_second,'\0',sizeof(meal_second));
}

void Handler(int signum)
{
    stop = 1;
    while(waitpid(-1, NULL, WNOHANG) > 0);
    exit(0);
}
