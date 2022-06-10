#include "socket_utils.h"

int createServerSock(int port, int type)
{
  int s, yes = 1;
  struct sockaddr_in sin;

  memset(&sin, 0, sizeof(sin)); // Init sin
  sin.sin_family = AF_INET;
  sin.sin_addr.s_addr = INADDR_ANY;
  sin.sin_port = htons((unsigned short)port);

  if (type == TRANSPORT_TYPE_TCP)
  {
    s = socket(PF_INET, SOCK_STREAM, 0);
  }
  else if (type == TRANSPORT_TYPE_UDP)
  {
    s = socket(PF_INET, SOCK_DGRAM, 0);
  }
  else
  {
    perror("Wrong transport type. Must be \"udp\" or \"tcp\"\n");
    return -1;
  }

  if (s < 0)
  {
    perror("Can't create socket\n");
    return -1;
  }

  setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes));

  if (bind(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
  {
    perror("Can't bind to port\n");
    return -1;
  }

  if (type == TRANSPORT_TYPE_TCP)
  {
    if (listen(s, 10) < 0)
    {
      perror("Can'n listen on port\n");
      return -1;
    }
  }

  return s;
}

int createClientSock(const char *host, int port, int type)
{
  int s;
  struct sockaddr_in sin;
  struct hostent *host_info;

  memset(&sin, 0, sizeof(sin)); // Init sin
  sin.sin_family = AF_INET;

  if((host_info = gethostbyname(host))){
    memcpy(&sin.sin_addr, host_info->h_addr_list[0], host_info->h_length);
  }else{
    sin.sin_addr.s_addr = inet_addr(host);
  }
  sin.sin_port = htons((unsigned short)port);

  if (type == TRANSPORT_TYPE_TCP)
  {
    s = socket(AF_INET, SOCK_STREAM, 0);
  }
  else if (type == TRANSPORT_TYPE_UDP)
  {
    s = socket(AF_INET, SOCK_DGRAM, 0);
  }
  else
  {
    perror("Wrong transport type. Must be \"udp\" or \"tcp\"\n");
    return -1;
  }

  if (type == TRANSPORT_TYPE_TCP)
  {
    if (connect(s, (struct sockaddr *)&sin, sizeof(sin)) < 0)
    {
      char msg_buf[30];
      sprintf(msg_buf, "Can'n connect to %s:%d\n", host, port);
      perror(msg_buf);
      return -1;
    }
  }
  return s;
}