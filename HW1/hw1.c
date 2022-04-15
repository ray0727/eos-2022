#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <sys/ioctl.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "asm-arm/arch-pxa/lib/creator_pxa270_lcd.h"

struct restaurant{
    int distance;
    int price1;
    int price2;
};
char keypad(int);
void decimal_to_segment(int, int);
void distance_to_led(int);
void order(int);
void pick_meal(int);
int state = 0, state_order=0;
struct restaurant dessert, beverage, diner;


int main(){
    int fd, ret;
    bool end = false;
    lcd_write_info_t display;
    char op, flag;
    int index=0, value;
    unsigned short key;

    dessert.distance = 3;
    dessert.price1 = 60;
    dessert.price2 = 80;
    beverage.distance = 5;
    beverage.price1 = 40;
    beverage.price2 = 70;
    diner.distance = 8;
    diner.price1 = 120;
    diner.price2 = 50;
    if((fd = open("/dev/lcd", O_RDWR))<0){
        printf("Open /dev/lcd failed.\n");
        exit(-1);
    }
    printf("Start\n");
    /*Start*/
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    display.Count = sprintf((char *) display.Msg, "1.shop list\n2.order");
    ioctl(fd, LCD_IOCTL_WRITE, &display);
    while(1){
        if(state==0){
            op = keypad(fd);
            if(op=='#'){
                if(flag=='1')
                    state = 1;
                else if(flag=='2')
                    state = 2;
            }
            else{
                flag = op;
                if(flag=='1' || flag=='2'){
                    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                    display.Count = sprintf((char *) display.Msg, "%c", op);
                    ioctl(fd, LCD_IOCTL_WRITE, &display);
                }
            }
        }
        else if(state==1){
            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
            display.Count = sprintf((char *) display.Msg, "Dessert Shop:3km\nBeverage Shop:5km\nDiner:8km\n");
            ioctl(fd, LCD_IOCTL_WRITE, &display);
            op = keypad(fd);
            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
            display.Count = sprintf((char *) display.Msg, "1.shop list\n2.order");
            ioctl(fd, LCD_IOCTL_WRITE, &display);
            state = 0;
        }
        else if(state==2){
            order(fd);
        }
        
        
    }
    close(fd);
    return 0;
}

char keypad(int fd){
    unsigned short key;
    int ret;
    while(1){
        ret = ioctl(fd, KEY_IOCTL_CHECK_EMTPY, &key);
        if(ret<0){
            sleep(0.5);
            continue;
        }
        ret = ioctl(fd, KEY_IOCTL_GET_CHAR, &key);
        return (key & 0xff);
    }
}

void order(int fd){
    char op, flag_order;
    lcd_write_info_t display;
    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
    display.Count = sprintf((char *) display.Msg, "Please choose 1~3\n1.Dessert Shop\n2.Beverage Shop\n3.Diner\n");
    ioctl(fd, LCD_IOCTL_WRITE, &display);
    while(1){
        if(state==0){
            printf("Back to Menu\n");
            break;
        }
        op = keypad(fd);
        if(op=='#'){
            if(flag_order=='1'){
                state_order = 1;
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "Please choose 1~4\n1.cookie 60$\n2.cake 80$\n3.confirm\n4.cancel\n");
                ioctl(fd, LCD_IOCTL_WRITE, &display);
                pick_meal(fd);
                
            }
                
            else if(flag_order=='2'){
                state_order = 2;
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "Please choose 1~4\n1.tea 40$\n2.boba 70$\n3.confirm\n4.cancel\n");
                ioctl(fd, LCD_IOCTL_WRITE, &display);
                pick_meal(fd);
            }
            else if(flag_order=='3'){
                state_order = 3;
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "Please choose 1~4\n1.fried rice 120$\n2.Egg-drop soup 50$\n3.confirm\n4.cancel\n");
                ioctl(fd, LCD_IOCTL_WRITE, &display);
                pick_meal(fd);
            }
        }
        else{
            flag_order = op;
            if(flag_order=='1' || flag_order=='2' || flag_order=='3'){
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "%c", op);
                ioctl(fd, LCD_IOCTL_WRITE, &display);
            }
        }  
    } 
}

void pick_meal(int fd){
    lcd_write_info_t display;
    char op, result[100]={0}, choose_meal;
    int index = 0, total_price=0, price1, price2;
    if(state_order==1){
        price1 = dessert.price1;
        price2 = dessert.price2;
    }
    else if(state_order==2){
        price1 = beverage.price1;
        price2 = beverage.price2;
    }
    else if(state_order==3){
        price1 = diner.price1;
        price2 = diner.price2;
    }
    while(1){
        op = keypad(fd);
        if(op=='#'){
            if(choose_meal=='1'){
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "How many?\n");
                ioctl(fd, LCD_IOCTL_WRITE, &display);
                while(1){
                    op = keypad(fd);
                    if(op=='#'){
                        result[index++] = '\0';
                        index = 0;
                        total_price+=atoi(result)*price1;
                        if(state_order==1){
                            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                            display.Count = sprintf((char *) display.Msg, "Please choose 1~4\n1.cookie 60$\n2.cake 80$\n3.confirm\n4.cancel\n");
                            ioctl(fd, LCD_IOCTL_WRITE, &display);
                        }
                        else if(state_order==2){
                            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                            display.Count = sprintf((char *) display.Msg, "Please choose 1~4\n1.tea 40$\n2.boba 70$\n3.confirm\n4.cancel\n");
                            ioctl(fd, LCD_IOCTL_WRITE, &display);
                        }
                        else if(state_order==3){
                            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                            display.Count = sprintf((char *) display.Msg, "Please choose 1~4\n1.fried rice 120$\n2.Egg-drop soup 50$\n3.confirm\n4.cancel\n");
                            ioctl(fd, LCD_IOCTL_WRITE, &display);
                        }
                        break;
                    }
                    else{
                        display.Count = sprintf((char *) display.Msg, "%c", op);
                        ioctl(fd, LCD_IOCTL_WRITE, &display);
                        result[index++] = op;
                    }
                }
            }
                
            else if(choose_meal=='2'){
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "How many?\n");
                ioctl(fd, LCD_IOCTL_WRITE, &display);
                while(1){
                    op = keypad(fd);
                    if(op=='#'){
                        result[index++] = '\0';
                        index = 0;
                        total_price+=atoi(result)*price2;
                        if(state_order==1){
                            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                            display.Count = sprintf((char *) display.Msg, "Please choose 1~4\n1.cookie 60$\n2.cake 80$\n3.confirm\n4.cancel\n");
                            ioctl(fd, LCD_IOCTL_WRITE, &display);
                        }
                        else if(state_order==2){
                            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                            display.Count = sprintf((char *) display.Msg, "Please choose 1~4\n1.tea 40$\n2.boba 70$\n3.confirm\n4.cancel\n");
                            ioctl(fd, LCD_IOCTL_WRITE, &display);
                        }
                        else if(state_order==3){
                            ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                            display.Count = sprintf((char *) display.Msg, "Please choose 1~4\n1.fried rice 120$\n2.Egg-drop soup 50$\n3.confirm\n4.cancel\n");
                            ioctl(fd, LCD_IOCTL_WRITE, &display);
                        }
                        break;
                    }
                    else{
                        display.Count = sprintf((char *) display.Msg, "%c", op);
                        ioctl(fd, LCD_IOCTL_WRITE, &display);
                        result[index++] = op;
                    }
                }
            }
            else if(choose_meal=='3'){
                if(atoi(result)==0){
                    state = 0;
                    printf("No meal\n");
                    ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                    display.Count = sprintf((char *) display.Msg, "1.shop list\n2.order");
                    ioctl(fd, LCD_IOCTL_WRITE, &display);
                    break;
                }
                printf("Total: %d\n", total_price);
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "Please wait for few minutes\n");
                ioctl(fd, LCD_IOCTL_WRITE, &display);
                decimal_to_segment(fd, total_price);
                distance_to_led(fd);
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "Please pick up your meal\n");
                ioctl(fd, LCD_IOCTL_WRITE, &display);
                op = keypad(fd);
                state = 0;
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "1.shop list\n2.order");
                ioctl(fd, LCD_IOCTL_WRITE, &display);
                break;
            }
            else if(choose_meal=='4'){
                state = 0;
                printf("Cancel\n");
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "1.shop list\n2.order");
                ioctl(fd, LCD_IOCTL_WRITE, &display);
                break;
            }
        }
        else{
            choose_meal = op;
            if(choose_meal=='1' || choose_meal=='2' || choose_meal=='3' || choose_meal=='4'){
                ioctl(fd, LCD_IOCTL_CLEAR, NULL);
                display.Count = sprintf((char *) display.Msg, "%c", op);
                ioctl(fd, LCD_IOCTL_WRITE, &display);
            }
        }
    }
    return;
}

void decimal_to_segment(int fd, int num){
    int i;
    _7seg_info_t data;
    unsigned long result=0;
    for(i=1; i<=4096; i*=16){
        result += (num%10)*i;
        num = num/10;
    }
    // printf("%lx\n", result);
    data.Mode = _7SEG_MODE_HEX_VALUE;
    data.Which = _7SEG_ALL;
    data.Value = result;
    ioctl(fd, _7SEG_IOCTL_SET, &data);
}

void distance_to_led(int fd){
    int i, distance;
    int data;
    data = 0x00;
    ioctl(fd, LED_IOCTL_SET, &data);
    if(state_order==1){
        distance = dessert.distance;
    }
    else if(state_order==2){
        distance = beverage.distance;
    } 
    else if(state_order==3){
        distance = diner.distance;
    }
    for(i=0; i<distance; i++){
        data = i;
        ioctl(fd, LED_IOCTL_BIT_SET, &data);
    }
    
    for(i=distance-1; i>=0; i--){
        data = i;
        sleep(1);
        ioctl(fd, LED_IOCTL_BIT_CLEAR, &data);
    }
        
}
