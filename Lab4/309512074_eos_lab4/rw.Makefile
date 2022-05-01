CC=arm-linux-gnueabihf-g++

all:
	${CC} -o reader reader.cpp --static
	${CC} -o writer writer.cpp --static
clean:
	${RM} writer reader
