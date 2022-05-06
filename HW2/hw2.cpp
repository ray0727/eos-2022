#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <string>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include "socket_utils.h"
#include <semaphore.h>
#include <vector>
#include <algorithm>
#include <cstdint>
#include <errno.h>

#define BUFSIZE 1024
using namespace std;

struct restaurant{
    char meal1[100], meal2[100];
    int distance;
    int price1;
    int price2;
};

int meal_type=0, first_meal_type, state = 0, state_order=0, first_order, total_num[6]={0,0,0,0,0,0};
bool first = true;
char result[BUFSIZE], meal_first[BUFSIZE], meal_second[BUFSIZE];
struct restaurant dessert, beverage, diner;

void order(int);
void pick_meal(char *, char *, int);
void reset();
void *process(void *t);



int main(int argc, char *argv[])
{
    int sockfd, connfd;
    struct sockaddr_in cln;
    socklen_t cln_len = sizeof(cln);
    int pth_c, p;
    pthread_t threadd,thr;
    int t=0;

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
    
    if(argc != 2){
        printf("Usage: %s <port>\n", argv[0]);
        exit(-1);
    }
    sockfd = createServerSock(atoi(argv[1]), TRANSPORT_TYPE_TCP);
    
    if (sockfd < 0)
    {
        perror("Error create socket\n");
        exit(-1);
    }

    while(1){
        connfd = accept(sockfd, ( struct sockaddr *) &cln, &cln_len);

        pth_c = pthread_create(&threadd, NULL, process, (void *)connfd);
        
        if(pth_c)
        {
            printf("ERROR return code from pthread_create( ) is %d\n", pth_c);
            exit(-1);
        }   
    }
    close(sockfd);
    return 0;
}

void *process(void *t)
{
    char send[BUFSIZE], rcv[BUFSIZE], op[100];
    char *p, *order[3];
    char delim[] = " ";
    int connfd = (intptr_t)t;
    int pay, total_meals=0;
    int n, i=0, j=0;
    bool con =false;
    /* Start */
    while(1)
    {
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
        strcpy(op, rcv);
        p = strtok(op, delim);
        while(p!=NULL){
            order[i++] = p;
            p = strtok(NULL, delim);  
        }
        i=0;
        if(strcmp(order[0], "shop")==0){
            state = 1;
            memset(send,'\0',sizeof(send));
            n = sprintf(send, "Dessert Shop:3km\n- cookie:60$|cake:80$\n\
Beverage Shop:5km\n- tea:40$|boba:70$\n\
Diner:8km\n- fried-rice:120$|Egg-drop-soup:50$%c", '\0');
            if((n = write(connfd, send, n)) == -1){
                printf("Error: write()\n");
                exit(-1);
            }
        }
        else if(strcmp(order[0], "order")==0){
            state = 2;
            pick_meal(order[1], order[2], connfd);
        }
        else if(strcmp(order[0], "confirm")==0){
            state = 3;
            for(j=0; j<6; j++){
                total_meals+=total_num[j];
            }
            if(total_meals==0){
                memset(send,'\0',sizeof(send));
                n = sprintf(send, "Please order some meals%c", '\0');
                if((n = write(connfd, send, n)) == -1){
                    printf("Error: write()\n");
                    exit(-1);
                }
                continue;
            }
            memset(send,'\0',sizeof(send));
            n = sprintf(send, "Please wait a few minutes...%c", '\0');
            if((n = write(connfd, send, n)) == -1){
                printf("Error: write()\n");
                exit(-1);
            }
            if(first_order==1){
                sleep(dessert.distance);
                pay = total_num[0]*dessert.price1 +total_num[1]*dessert.price2;
            }
            else if(first_order==2){
                sleep(beverage.distance);
                pay = total_num[2]*beverage.price1 +total_num[3]*beverage.price2;
            }
            else if(first_order==3){
                sleep(diner.distance);
                pay = total_num[4]*diner.price1 +total_num[5]*diner.price2;
            }
            memset(send,'\0',sizeof(send));
            n = sprintf(send, "Delivery has arrived and you need to pay %d$%c", pay, '\0');
            if((n = write(connfd, send, n)) == -1){
                printf("Error: write()\n");
                exit(-1);
            }
            reset();
            pay = 0;
            total_meals = 0;
        }
        else if(strcmp(order[0], "cancel")==0){
            reset();
            pay = 0;
            total_meals = 0;
        }
    }
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
        printf("Wrong Resturant\n");
        n = strlen(result);
    }
    else if(meal_type == first_meal_type){
        printf("Same meal\n");
        n = sprintf(meal_first, "%s %d%c", meal, total_num[meal_type],'\0');
    }
    else{
        printf("Same Resturant, different meal\n");
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