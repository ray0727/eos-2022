CXX=g++
CFLAGS=-Wall -pthread -std=c++11

CC_ARM=arm-unknown-linux-gnu-gcc
CFLAGS_ARM=-L /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/lib/ \
	   -I /opt/arm-unknown-linux-gnu/arm-unknown-linux-gnu/include/ \
	   -I /home/ezra/microtime/linux/include/ \
	   -pthread

all:
	${CXX} ${CFLAGS} -c game.cpp
	${CXX} ${CFLAGS} -c main.cpp
	${CXX} ${CFLAGS} -c socket_utils.c
	${CXX} ${CFLAGS} -c random_map.c
	${CXX} ${CFLAGS} main.o socket_utils.o game.o random_map.o -o game
	${CC_ARM} ${CFLAGS_ARM} random_map.c LCD.c client.cpp game_client.cpp sockop.cpp -o client
clean:
	${RM} main.o game.o socket_utils.o game client
