#ifndef SOCKET_UTILS_H
#define SOCKET_UTILS_H

#include <fcntl.h>  // open()
#include <unistd.h>
#include <string.h>
#include <sys/socket.h> // socket(), connect()
#include <netinet/in.h> // struct sockaddr_in
#include <arpa/inet.h> // inet_addr()
#include <netdb.h>
#include <stdio.h>
#include <errno.h>

#define TRANSPORT_TYPE_TCP 0
#define TRANSPORT_TYPE_UDP 1

int createServerSock(int port, int type);
int createClientSock(const char* host, int port, int type);

#endif