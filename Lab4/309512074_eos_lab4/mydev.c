#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
MODULE_LICENSE("GPL");


char data='0';
char *cmd;
char output[16];

static char seg_for_c[27][17] = {
    "1111001100010001", // A
    "0000011100000101", // b
    "1100111100000000", // C
    "0000011001000101", // d
    "1000011100000001", // E
    "1000001100000001", // F
    "1001111100010000", // G
    "0011001100010001", // H
    "1100110001000100", // I
    "1100010001000100", // J
    "0000000001101100", // K
    "0000111100000000", // L
    "0011001110100000", // M
    "0011001110001000", // N
    "1111111100000000", // O
    "1000001101000001", // P
    "0111000001010000", //q
    "1110001100011001", //R
    "1101110100010001", //S
    "1100000001000100", //T
    "0011111100000000", //U
    "0000001100100010", //V
    "0011001100001010", //W
    "0000000010101010", //X
    "0000000010100100", //Y
    "1100110000100010", //Z
    "0000000000000000"  //0
};

// File Operations
static ssize_t my_read(struct file *fp, char *buf, size_t count, loff_t *fpos)
{
    printk("call_read\n");
    switch(data)
    {
        case 'A':
        case 'a':
            cmd = seg_for_c[0];
            break;
        case 'b':
        case 'B':
            cmd = seg_for_c[1];
            break;
        case 'C':
        case 'c':
            cmd = seg_for_c[2]; 
            break;
        case 'd':
        case 'D':
            cmd = seg_for_c[3];
            break;
        case 'E':
        case 'e':
            cmd = seg_for_c[4];
            break;
        case 'F':
        case 'f':
            cmd = seg_for_c[5];
            break;
        case 'G':
        case 'g':
            cmd = seg_for_c[6];
            break;
        case 'H':
        case 'h':
            cmd = seg_for_c[7];
            break;
        case 'I':
        case 'i':
            cmd = seg_for_c[8];
            break;
        case 'J':
        case 'j':
            cmd = seg_for_c[9];
            break;
        case 'K':
        case 'k':
            cmd = seg_for_c[10];
            break; 
        case 'L':
        case 'l':
            cmd = seg_for_c[11];
            break;  
        case 'M':
        case 'm':
            cmd = seg_for_c[12];
            break;
        case 'N':
        case 'n':
            cmd = seg_for_c[13];
            break;
        case 'O':
        case 'o':
            cmd = seg_for_c[14];
            break;
        case 'P':
        case 'p':
            cmd = seg_for_c[15];
            break;      
        case 'q':
        case 'Q':
            cmd = seg_for_c[16];
            break; 
        case 'R':
        case 'r':
            cmd = seg_for_c[17];
            break; 
        case 'S':
        case 's':
            cmd = seg_for_c[18];
            break; 
        case 'T':
        case 't':
            cmd = seg_for_c[19];
            break; 
        case 'U':
        case 'u':
            cmd = seg_for_c[20]; 
            break;
        case 'V':
        case 'v':
            cmd = seg_for_c[21]; 
            break;
        case 'W':
        case 'w':
            cmd  = seg_for_c[22];
            break;
        case 'X':
        case 'x':
            cmd  = seg_for_c[23]; 
            break; 
        case 'Y':
        case 'y':
            cmd  = seg_for_c[24];
            break;
        case 'Z':
        case 'z':
            cmd = seg_for_c[25];
            break;  
        default:
            cmd = seg_for_c[26];
            break;
    }
    int i;
    for(i=0;i<16;i++)
    {
        output[i] = cmd[i]-48;
    }
    copy_to_user(buf,output,16);

    return count;
}

static ssize_t my_write(struct file *fp, const char *buf, size_t count, loff_t *fpos)
{
    printk("call_write\n");
    copy_from_user(&data,buf,1);
    return count;
}

static int my_open(struct inode *inode, struct file *fp)
{
    printk("call_open\n");
    return 0;
}


struct file_operations my_fops={
    read: my_read,
    write: my_write,
    open: my_open
};

#define MAJOR_NUM 244
#define DEVICE_NAME "my_dev"

static int my_init(void)
{
    printk("call_init\n");
    if(register_chrdev(MAJOR_NUM, DEVICE_NAME, &my_fops) < 0)
    {
        printk("Can_not_get_major_%d\n", MAJOR_NUM);
        return(-EBUSY);
    }

    printk("My_device_is_started_and_the_major_is_%d\n", MAJOR_NUM);
    return 0;
}

static void my_exit(void)
{
    unregister_chrdev(MAJOR_NUM, DEVICE_NAME);
    printk("call_exit\n");
}

module_init(my_init);
module_exit(my_exit);