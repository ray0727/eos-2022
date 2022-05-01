#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/fcntl.h>
#include <unistd.h>

int main(int argc, char const *argv[])
{
    if (argc != 2)
    {
        printf("please input testcase.\n");
        exit(0);
    }

    printf("%s %d\n", argv[1], strlen(argv[1]));

    int fd;
    if ((fd = open("/dev/mydev", O_RDWR)) < 0)
    {
        printf("Error open %s\n", "/dev/mydev");
        exit(-1);
    }

    int i, ret;
    char digit;
    while(1){
        for(i=0;i<strlen(argv[1]);i++){
            digit = argv[1][i];
            ret = write(fd,&digit,1);
            printf("%c\n",digit);
            sleep(1);
        }
    }

    return 0;
}