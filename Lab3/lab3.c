#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"


char interpret_key(unsigned short);
int calculate_recursive(char*);
void decimal_to_segment(int, int);
void decimal_to_led(int, int);


int main(){
    int fd, ret;
    bool end = false;
    lcd_write_info_t display;
    char result[100] = {0}, op;
    int index=0, value;
    unsigned short key;
    if((fd = open("/dev/lcd", O_RDWR))<0){
        printf("Open /dev/lcd failed.\n");
        exit(-1);
    }
    /*Clear LCD*/
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    printf("Start calculator\n");

    /*Start*/
    while(1){
        ret = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);
        if(ret<0){
            sleep(1);
            continue;
        }
        ret = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
        if(end){
            end = false;
            display.Count = sprintf((char *)display.Msg, "");
            index = 0;
            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
            ioctl(fd, LCD_IOCTL_WRITE, &display);
        }
        op = interpret_key(key);
        
        if(op=='#'){
            display.Count = sprintf((char *) display.Msg, "=");
            ioctl(fd, LCD_IOCTL_WRITE, &display);
            result[index++] = '\0';
            value = calculate_recursive(result);
            display.Count = sprintf((char *) display.Msg, "%d", value);
            end = true;
            decimal_to_segment(fd, value);
            decimal_to_led(fd, value);
        }
        else if((key&0xff) == '*'){
            display.Count = sprintf((char *) display.Msg, "");
            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
            index = 0;
        }
        else{
            display.Count = sprintf((char *) display.Msg, "%c", op);
            result[index++] = op;
        }
        
        ioctl(fd, LCD_IOCTL_WRITE, &display);
    }
    close(fd);
    return 0;
}

char interpret_key(unsigned short k){
    switch(k & 0xff)
    {
        case 'A': return '+';
        case 'B': return '-';
        case 'C': return '*';
        case 'D': return '/';
        default: return (k & 0xff);
    }
}

int calculate_recursive(char* operations)
{
    char* pos = NULL;
    char str1[100], str2[100];

    if((pos = strchr(operations, '+')) != NULL){
        strcpy(str1, operations);
        strcpy(str2, pos+1);
        str1[pos-operations] = '\0';
        printf("%s %c %s\n", str1, *pos, str2);
        return calculate_recursive(str2) + calculate_recursive(str1);
    }
    if((pos = strchr(operations, '-')) != NULL){
        strcpy(str1, operations);
        strcpy(str2, pos+1);
        str1[pos-operations] = '\0';
        printf("%s %c %s\n", str1, *pos, str2);

        return -(calculate_recursive(str2) - calculate_recursive(str1));
    }
    if((pos = strchr(operations, '*')) != NULL){
        strcpy(str1, operations);
        strcpy(str2, pos+1);
        str1[pos-operations] = '\0';
        printf("%s %c %s\n", str1, *pos, str2);

        return calculate_recursive(str2) * calculate_recursive(str1);
    }
    if((pos = strchr(operations, '/')) != NULL){
        strcpy(str1, operations);
        strcpy(str2, pos+1);
        str1[pos-operations] = '\0';
        printf("%s %c %s\n", str1, *pos, str2);
        return 1/((float)calculate_recursive(str2) / calculate_recursive(str1));
    }
    return atoi(operations);
}

void decimal_to_segment(int fd, int num){
    int i;
    _7seg_info_t data;
    unsigned long result=0;
    for(i=1; i<=4096; i*=16){
        result += (num%10)*i;
        num = num/10;
    }
    printf("%lx\n", result);
    data.Mode = _7SEG_MODE_HEX_VALUE;
    data.Which = _7SEG_ALL;
    data.Value = result;
    ioctl(fd, _7SEG_IOCTL_SET, &data);
}

void decimal_to_led(int fd, int num){
    int i, data;
    ioctl(fd, LED_IOCTL_SET, &data);
    for(i=0; i<8; i++){
        data = 7-i;
        if(num%2==1) ioctl(fd, LED_IOCTL_BIT_SET, &data);
        else ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
        num = num/2;
    }
}
