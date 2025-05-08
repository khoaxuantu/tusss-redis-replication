#include "./connections/index.h"
#include "./connections/poller.hpp"

int main(void)
{
  int fd = conn_init_socket();
  poller_poll(fd);
}
