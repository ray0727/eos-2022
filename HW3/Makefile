CXX=gcc
CFLAGS=-lpthread

all:
	${CXX} ${CFLAGS} -c hw3.c
	${CXX} ${CFLAGS} -c socket_utils.c
	${CXX} hw3.o socket_utils.o ${CFLAGS} -o hw3

clean:
	${RM} socket_utils.o hw3.o hw3