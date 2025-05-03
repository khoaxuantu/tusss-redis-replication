#include "../libs/helpers.h"
#include "arpa/inet.h"
#include "netinet/ip.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/socket.h"
#include "unistd.h"

struct sockaddr_in init_addr()
{
  struct sockaddr_in addr = {};

  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(0);

  return addr;
}

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
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    die("socket()");
  }

  int val = 1;
  setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

  struct sockaddr_in addr = init_addr();

  int rv = bind(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv)
  {
    fprintf(stderr, "Bind returns %d\n", rv);
    die("bind()");
  }

  rv = listen(fd, SOMAXCONN);
  if (rv)
  {
    die("listen()");
  }

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
