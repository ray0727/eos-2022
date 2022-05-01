CC=arm-linux-gnueabihf-gcc

obj-m:=mydev.o

ifndef  ZC_REPO
	ZC_REPO=/home/ray
endif

all:
	make -C $(ZC_REPO)/linux-xlnx M=$(PWD) modules
clean:
	make -C $(ZC_REPO)/linux-xlnx M=$(PWD) clean
