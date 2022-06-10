#ifndef SOCKOP_H
#define SOCKOP_H

#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>

#define errexit(format ,arg...) exit(printf(format,##arg))

/* Create server */
int passivesock(const char *service ,const char *transport , int qlen);
/* Connect to server */
int connectsock( const char *host , const char *service, const char *transport);

#endif /* _SOCKOP_H_ */
