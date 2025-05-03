#include "../../libs/helpers.h"
#include "arpa/inet.h"

#ifndef CONNECTION_H
#define CONNECTION_H

int _init_socket()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    die("socket()");
  }

  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  return fd;
}

static struct sockaddr_in _init_addr()
{
  struct sockaddr_in addr = {};

  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(0);

  return addr;
}

void _bind(int *fdPtr)
{
  struct sockaddr_in addr = _init_addr();
  int rv = bind(*fdPtr, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv)
  {
    die("bind()");
  }
}

void _listen(int *fdPtr)
{
  int rv = listen(*fdPtr, SOMAXCONN);
  if (rv)
  {
    die("listen()");
  }
}

int conn_init_socket()
{
  int fd = _init_socket();
  _bind(&fd);
  _listen(&fd);

  return fd;
}

#endif
