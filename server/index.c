#include "./connections/index.h"
#include "./connections/poller.h"
#include "stdbool.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "sys/socket.h"
#include "unistd.h"

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

    poller_polling(connfd);

    close(connfd);
  }
}
