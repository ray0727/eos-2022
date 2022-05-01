#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "socket_utils.h"

int main(int argc, char **argv)
{
  int conn_fd, msg_cnt, i;
  char action = 0;
  char msg_buf[50];

  if (argc != 6)
  {
    printf("Usage: %s <host> <port> <deposit/withdraw> <amount> <times>\n", argv[0]);
    exit(-1);
  }

  if (strcmp(argv[3], "deposit") == 0)
  {
    action = 'D';
  }
  else if (strcmp(argv[3], "withdraw") == 0)
  {
    action = 'W';
  }
  else
  {
    sprintf(msg_buf, "invalid action %s\n", argv[3]);
    perror(msg_buf);
    exit(-1);
  }
  msg_cnt = sprintf(msg_buf, "%c,%s", action, argv[4]);

  for (i = 0; i < atoi(argv[5]); i++)
  {
    conn_fd = createClientSock(argv[1], atoi(argv[2]), TRANSPORT_TYPE_TCP);
    write(conn_fd, msg_buf, msg_cnt);
    close(conn_fd);
  }

  return 0;
} 