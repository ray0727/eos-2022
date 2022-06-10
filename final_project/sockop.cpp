#include "sockop.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

/*
* passivesock - allocate & bind a server socket using TCP or UDP
*
 * Arguments :
 * service - service associated with the desired port
 * transport - transport protocol to use (" tcp" or "udp ")
 * qlen - maximum server request queue length
 */
int passivesock(const char *service , const char *transport , int qlen)
{
   struct servent *pse; /* pointer to service information entry */
   struct sockaddr_in sin; /* an Internet endpoint address */
   int s , type; /* socket descriptor and socket type */

   memset(&sin , 0 , sizeof(sin));
   sin.sin_family = AF_INET ;
   sin.sin_addr.s_addr = INADDR_ANY;

 /* Map service name to port number */
   if((pse = getservbyname(service, transport)) )
      sin.sin_port = htons( ntohs((unsigned short)pse -> s_port));
   else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0)
      errexit("can't find \"%s \" service entry \n" , service);

 /* Use protocol to choose a socket type */
   if( strcmp(transport , "udp" ) == 0 )
      type = SOCK_DGRAM;
   else
      type = SOCK_STREAM;

 /* Allocate a socket */
   s = socket(PF_INET , type , 0);
   if( s < 0 )
      errexit( "Can't create socket : %s \n" ,strerror (errno));

 /* Bind the socket */
   if(bind(s ,(struct sockaddr *)&sin , sizeof( sin )) < 0 )
   errexit( "Can't  bind to port\n");

 /* Set the maximum number of waiting connection */
   if(type == SOCK_STREAM && listen(s , qlen) < 0 )
   errexit( "Can't listen on port \n");

   return s ;
}

 /*
 * connectsock - allocate & connect a socket using TCP or UDP
 *
 * Arguments :
 * host - name of host to which connection is desired
 * service - service associated with the desired port
 * transport - name of transport protocol to use (" tcp" or "udp ")
 */
int connectsock( const char *host , const char *service , const char *transport)
{
   struct hostent *phe ; /* pointer to host information entry */
   struct servent *pse ; /* pointer to service information entry */
   struct sockaddr_in sin ; /* an Internet endpoint address */
   int s , type ; /* socket descriptor and socket type */

   memset(&sin , 0 , sizeof(sin));
   sin.sin_family = AF_INET;

   /* Map service name to port number */
   if( (pse = getservbyname( service , transport)))
     sin.sin_port = pse->s_port;
   else if((sin.sin_port = htons((unsigned short)atoi(service))) == 0 )
     errexit("Can't get \"%s \" service entry \n" , service);

   /* Map host name to IP address , allowing for dotted decimal */
   if((phe = gethostbyname(host)))
     memcpy(&sin.sin_addr , phe->h_addr , phe->h_length);
   else if(( sin.sin_addr.s_addr = inet_addr(host)) == INADDR_NONE)
     errexit( "Can't get \"%s \" host entry\n" , host);

   /* Use protocol to choose a socket type */
   if( strcmp(transport , "udp" ) == 0 )
     type = SOCK_DGRAM;
   else
     type = SOCK_STREAM;

   /* Allocate a socket */
   s = socket(PF_INET, type, 0 );
   if( s < 0 )
     errexit("Can't create socket : %s \n" ,strerror(errno));

   /* Connect the socket */
   if( connect( s ,(struct sockaddr *)&sin , sizeof(sin) ) < 0 )
     errexit("Can't connect to service");
   return s ;
}
