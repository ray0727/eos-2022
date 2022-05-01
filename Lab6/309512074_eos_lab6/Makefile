CC=gcc
CFLAGS=-Wall
ING_FLAGS=-lpthread

all: server.o client.o socket_utils.o
	${CC} ${CFLAGS} -c server.c
	${CC} ${CFLAGS} -c client.c
	${CC} ${CFLAGS} -c socket_utils.c
	${CC} client.o socket_utils.o ${CFLAGS} ${ING_FLAGS} -o client
	${CC} server.o socket_utils.o ${CFLAGS} ${ING_FLAGS} -o server

clean: 
	${RM} server client server.o client.o socket_utils.o
