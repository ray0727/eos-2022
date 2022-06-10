#ifndef GAMECLIENT_H
#define GAMECLIENT_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/sem.h>
#include <string.h>
#include "data_utils.h"
#include "random_map.h"
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

class GameClient
{
    public:
        GameClient();
        ~GameClient();
        void readServer();
        void run();
        void setup(int ,int);
    private:
        void sendServer();
        void draw_graph(int ,int ,int );
        void draw_cards(int[] );
        void draw();
        void show_7SEG(int, int);
        int read_pad();
    
    private:
        char sndmsg[100];
        char sndmsg2[100];
        char rcvmsg[100];
        char keypad_input[2];
        int temp[ROL_SIZE][COL_SIZE];
        int stage_pointer;
        int round_num;
        bool secondhit;
        bool hide_and_play;
        int lcd_semid;
        int server_fd,io_fd;
        int rcv_size;
        key_t key;
        lcd_full_image_info graph;
};

#endif 
