#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <string.h>
#include "game_client.h"
#include "LCD.h"
#include "random_map.h"

#define SEM_MODE 0666

int P(int s)
{
   struct sembuf sop;
   sop.sem_num = 0;
   sop.sem_op = -1;
   sop.sem_flg = 0;
   if(semop(s,&sop,1) < 0)
        exit(printf("sem error\n"));
   else
        return 0;
}
int V(int s)
{
   struct sembuf sop;
   sop.sem_num = 0;
   sop.sem_op = 1;
   sop.sem_flg = 0;
   if(semop(s,&sop,1) < 0)
        exit(printf("sem error\n"));
   else
        return 0;
   
}

// Show LCD monitor
void show_LCD_pic(int pattern[ROL_SIZE][COL_SIZE], int fd)
{
    lcd_full_image_info_t display;  // struct for saving picture
    // Clear LCD
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    int i = 0, j = 0;
    // LCD row(16 x 16) col(128)
    for (i = 0; i < 2048; i++){        
        display.data[i] = all_hidden[i];
    }
    if (pattern[0][0] == SPACE){
        return;
    }
    // Card size = 20 x 72, only set 20 x 64
    int k, l;
    int r, c;
    int card_num, card_row, card_col;
    for (k = 0; k < ROL_SIZE; k++){
        for (l = 0; l < COL_SIZE; l++){
            card_num = pattern[k][l] - 1;
            card_row = start_pos[k][l][0];
            card_col = start_pos[k][l][1];
            r = card_row;
            c = card_col;
            for (j = 0; j < 20; j++){
                for (i = 0; i < 4; i++){
                    display.data[16 * r + c] = card[card_num][j][i];
                    c++;
                }
                c = card_col;
                r++;
            }
        }
    }
    // printf("sizeof display.data: %d\n", sizeof(display.data));
    ioctl(fd, LCD_IOCTL_DRAW_FULL_IMAGE, &display);
}

// Show the LED
void show_LED(int num, int fd)
{
    int i = 0;
    if (num > 8)
        printf("Error: 0 <= num <= 8\n");
    for (; i < num; i++){
        ioctl(fd, LED_IOCTL_BIT_SET, &i);
    }
    for (i = num; i < 8; i++){
        ioctl(fd, LED_IOCTL_BIT_CLEAR, &i);
    }
}

/* Constructer and Distructer */
GameClient::GameClient()
{
    this->stage_pointer = 2;
    this->keypad_input[0] = '\0';
    this->keypad_input[1] = '\0';
    this->secondhit = false;
    this->hide_and_play = false;//display section
    this->round_num = 0;
    this->key = 66600;
    /*Create Semaphore for lcd display*/ 
    while(1)
    {
        if((this->lcd_semid = semget(this->key, 1, IPC_CREAT|SEM_MODE))<0){
            printf("Semaphore get error!\n");
            if(this->key == 66610) /*Try 10 semaphore key*/
                exit(printf("Semaphore Create failed\n"));
            else
                this->key++;
        }
        else
        {
            if(semctl(this->lcd_semid,0,SETVAL,1)<0)
                exit(printf("Can't init semaphore...\n"));
            break;
        }
        
    }
    printf("finish init game....\n");
}
GameClient::~GameClient()
{
    semctl(this->lcd_semid,0,IPC_RMID,0);
}

void GameClient::setup(int server_fd, int io_fd)
{
    this->server_fd = server_fd;
    this->io_fd = io_fd;
}



/* Interaction with server 
* readServer() will keep listening to server and is called by other thread in main function
* msghandler() will execute the server's command depand on current stage. 
*/
void GameClient::readServer()
{
    while(1)
    {
        this->rcv_size = read(this->server_fd,this->rcvmsg, sizeof(this->rcvmsg));
        server_pkt pkt;
        memcpy(&pkt,this->rcvmsg,sizeof(server_pkt));
        printf("Receive Message...");
        int zero[ROL_SIZE][COL_SIZE]={1};
        this->hide_and_play = pkt.hide_p;
        if(this->stage_pointer == 2)
        {
            // this->read_pad();
            if (pkt.gameState == GAME_STATE_INIT)
            {
                show_LCD_pic(zero, this->io_fd);
            }
            else if(pkt.gameState == GAME_STATE_PLAYING)
            {
                if(pkt.newR == true){
                    this->secondhit = false;
                }//clear second hit for new round
                if(pkt.newS == true){
                    this->secondhit = false;
                }//clear second hit for new section
                this->round_num = pkt.round_num;
                printf("Server to Client: \n");
                for (int i = 0; i < ROL_SIZE; i++)
                {
                    for(int j=0;j<COL_SIZE;j++)
                    {
                        this->temp[i][j] = pkt.card_states[i*COL_SIZE+j];
                        printf("%d ", this->temp[i][j]);
                    }
                    printf("\n");
                }
                show_LCD_pic(this->temp, this->io_fd);
                // this->keypad_input[0] = 88;
                show_LED(ROUND_TIMES+1-this->round_num, this->io_fd);
                printf("Score = %d\n",pkt.score);
                this->show_7SEG(pkt.score, this->io_fd);
                if (this->round_num == ROUND_TIMES+1)
                    show_LCD_pic(zero, this->io_fd);
            }
            else if(pkt.gameState == GAME_STATE_END)
            {
                this->keypad_input[1] = '#';
                this->secondhit = false;
                show_LCD_pic(zero, this->io_fd);
                printf("game_state_end\n");
                return;
            }
            
        }
    }
}

void GameClient::show_7SEG(int result, int fd)
{
    printf("Showing %d on 7 SEG\n", result);
    _7seg_info_t seg_data;
    int i = 0, temp = result;
    unsigned long hex_result = 0;
    for (i = 1; i <= 4096; i*=16){
        hex_result += (temp % 10) * i;
        temp /= 10;
    }

    ioctl(fd, _7SEG_IOCTL_ON, NULL);
    seg_data.Mode = _7SEG_MODE_HEX_VALUE;
    seg_data.Which = _7SEG_ALL;
    seg_data.Value = hex_result;
    ioctl(fd, _7SEG_IOCTL_SET, &seg_data);

    return;
}

void GameClient::sendServer()
{
    write(this->server_fd, this->sndmsg, strlen(this->sndmsg));
    write(this->server_fd, this->sndmsg2, strlen(this->sndmsg));
}

int GameClient::read_pad()
{
    unsigned short key;
    if( ioctl(this->io_fd, KEY_IOCTL_CHECK_EMTPY, &key) < 0){
        usleep(100000); //Sleep 0.1s
        return 0;
    }
	ioctl(this->io_fd, KEY_IOCTL_GET_CHAR, &key);
    //change to char and print out
    this->keypad_input[0] = key & 0xff;
    this->keypad_input[1] = '\0';

    int i,j,tt;
    
    for (i = 0; i < ROL_SIZE; i++)
    {
        for(j=0;j<COL_SIZE;j++)
        {
            char key0;
            int pos;
            key0 = this->keypad_input[0];
            if((key0 > '0') && (key0 <= '9'))
                pos = key0-'1';
            else if(key0 == '*')
                pos = 9;
            else if(key0 == '0')
                pos = 10;
            else if(key0 == '#')
                pos = 11;
            if(i*COL_SIZE+j == pos){
                printf("\nhi\n");
                tt = this->temp[i][j];
                this->temp[i][j] = 7;
            }
        }
    }
    if(this->hide_and_play == true)
        show_LCD_pic(this->temp, this->io_fd);
    return 1;
}

void GameClient::run()
{
    while(1)
    {
        if(this->stage_pointer == 2)
        {
            printf("Stage 2\n");
            //this->draw_cards(0x00);
            int i;
            char key0;
            while(1)
            {
                if(this->read_pad())
                {
                    key0 = this->keypad_input[0];
                    if( ( (key0 > '0') && (key0 <= '9') ) || (key0 == '*') || (key0 == '#') || (key0 == '0'))
                    {   
                        if(this->secondhit == false){//first hit          
                            sprintf(this->sndmsg,"%c",key0);
                            this->secondhit = true;
                        }else{
                            sprintf(this->sndmsg2,"%c",key0);
                            this->secondhit = false;
                            this->sendServer();
                        }
                    }
                }
                if(this->keypad_input[1] == '#') // When time's up, change keypad_input[1] to '#'
                    return;
            }
            this->stage_pointer = 2;
        }
    }
}
