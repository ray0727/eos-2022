#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <math.h>
#include <time.h>
#include "LCD.h"
#include "random_map.h"

// show the 7SEG
void show_7SEG(int result, int *fd)
{
    printf("Showing %d on 7 SEG\n", result);
    _7seg_info_t seg_data;
    int i = 0, temp = result;
    unsigned long hex_result = 0;
    for (i = 1; i <= 4096; i*=16){
        hex_result += (temp % 10) * i;
        temp /= 10;
    }
    // Check HEX number
    // printf("HEX: %04lx\n", hex_result);

    ioctl(*fd, _7SEG_IOCTL_ON, NULL);
    seg_data.Mode = _7SEG_MODE_HEX_VALUE;
    seg_data.Which = _7SEG_ALL;
    seg_data.Value = hex_result;
    ioctl(*fd, _7SEG_IOCTL_SET, &seg_data);

    return;
}

// Show the LED
void show_LED(int num, int *fd)
{
    int i = 0;
    if (num > 8)
        printf("Error: 0 <= num <= 8\n");
    for (; i < num; i++){
        ioctl(*fd, LED_IOCTL_BIT_SET, &i);
    }
    for (i = num; i < 8; i++){
        ioctl(*fd, LED_IOCTL_BIT_CLEAR, &i);
    }
}

// Show LCD monitor
void show_LCD_msg(char *string, int *fd)
{
    lcd_write_info_t display;  //struct for saving LCD data
    // Clear LCD
    ioctl(*fd, LCD_IOCTL_CLEAR, NULL);
    // Save output string to display data structure
    display.Count = sprintf((char *)display.Msg, string);
    // print msg to LCD
    ioctl(*fd, LCD_IOCTL_WRITE, &display);
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



// int main(int argc, char *argv[])
// {
//     // Get pattern from random_map.c
//     clock_t start, finish;
//     // 0 means no pattern
//     // category number from 1~N
//     start = clock();
//     int pattern1[ROL_SIZE][COL_SIZE];
//     int pattern2[ROL_SIZE][COL_SIZE];
//     Get_uniform_pattern(pattern1, ROL_SIZE, COL_SIZE, CAT_NUM);
//     Get_uniform_pattern(pattern2, ROL_SIZE, COL_SIZE, CAT_NUM);
//     finish = clock();
//     printf("Generate time: %f\n", (double)(finish - start) / CLOCKS_PER_SEC);

//     int fd;    // file descriptor for /dev/lcd
//     // Open device /dev/lcd
//     if ((fd = open("/dev/lcd", O_RDWR)) < 0){
//         printf("Open /dev/lcd failed.\n");
//         exit(-1);
//     }
//     // Show pattern
//     printf("showing pattern1:\n");
//     print_pattern(pattern1);
//     show_LCD_pic(pattern1, &fd);
//     sleep(5);
//     // Show pattern
//     printf("showing pattern2:\n");
//     print_pattern(pattern2);
//     show_LCD_pic(pattern2, &fd);
//     sleep(5);

//     // Close
//     printf("Closing LCD\n");
//     ioctl(fd, LCD_IOCTL_CLEAR, NULL);
//     close(fd);
//     return 0;
// }
