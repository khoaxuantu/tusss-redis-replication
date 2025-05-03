#include "../libs/helpers.h"
#include "arpa/inet.h"
#include "stdio.h"
#include "string.h"
#include "sys/socket.h"
#include "unistd.h"

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

  char msg[] = "Hello from client";
  ssize_t err_check = write(fd, msg, strlen(msg));
  if (err_check < 0)
  {
    die("write");
  }

  char read_buffer[64] = {};
  ssize_t n = read(fd, read_buffer, sizeof(read_buffer) - 1);
  if (n < 0)
  {
    die("read()");
  }

  printf("Server says: %s\n", read_buffer);
  close(fd);
}
