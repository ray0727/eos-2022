CXX=g++
CFLAGS=-lpthread

all:
	${CXX} ${CFLAGS} -c hw2.cpp
	${CXX} ${CFLAGS} -c socket_utils.c
	${CXX} hw2.o socket_utils.o ${CFLAGS} ${INC_FLAGS} -o hw2

clean:
	${RM} hw2.o socket_utils.o hw2