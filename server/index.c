#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/socket.h"
#include "unistd.h"
#include "./connections/index.h"

static void do_something(int connfd)
{
  char rbuf[64] = {};

  ssize_t err_check = read(connfd, rbuf, sizeof(rbuf) - 1);
  if (err_check < 0)
  {
    perror("read() error");
    return;
  }

  printf("client says: %s\n", rbuf);

  char wbuf[] = "world";
  err_check = write(connfd, wbuf, strlen(wbuf));
  if (err_check < 0)
  {
    perror("write() error");
    return;
  }
}

int main(void)
{
  int fd = conn_init_socket();

  while (true)
  {
    struct sockaddr_in client_addr = {};
    socklen_t addr_len = sizeof(client_addr);
    int connfd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
    if (connfd < 0)
    { 
      continue;
    }

    do_something(connfd);
    close(connfd);
  }
}
