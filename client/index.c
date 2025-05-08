#include "../libs/helpers.h"
#include "./query.h"
#include "sys/socket.h"
#include "unistd.h"
#include <arpa/inet.h>

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    die("socket()");
  }

  struct sockaddr_in addr = {};
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK);
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);

  int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv)
  {
    die("connect()");
  }

  if (!query(fd, "hello1"))
  {
    goto L_DONE;
  }

  if (!query(fd, "hello234"))
  {
    goto L_DONE;
  }

L_DONE:
  close(fd);
  return 0;
}
