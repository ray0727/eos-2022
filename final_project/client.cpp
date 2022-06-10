#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"
#include "sockop.h"
#include "game_client.h"

GameClient game;
pthread_t thread;
int io_fd;
int socketfd;
unsigned short key;
lcd_write_info_t display;
int button;

void* server_Listener(void* argv)
{
    game.readServer();
}

int main(int argc, char** argv)
{
    printf("Start game now\n");
    printf("Size of short = %d\n",sizeof(unsigned short));
    if(argc != 3)
        exit(printf("Usage: %s [IP address] [port]\n",argv[0]));
    if((socketfd = connectsock(argv[1],argv[2],"tcp")) < 0)
        exit(printf("Connect failed!!!\n"));
    
    if((io_fd = open("/dev/lcd", O_RDWR)) < 0)
        exit(printf("Open LCD module failed!!!!!\n"));

    ioctl(io_fd, LCD_IOCTL_CUR_OFF, NULL);
    ioctl(io_fd, LCD_IOCTL_CLEAR, NULL);
    ioctl(io_fd, KEY_IOCTL_CLEAR, key);
    //game.setup(0, io_fd);
    game.setup(socketfd, io_fd);

    pthread_create(&thread,NULL,server_Listener,NULL);
    printf("Servrer listener created\n");
    pthread_detach(thread);

    game.run();

}
