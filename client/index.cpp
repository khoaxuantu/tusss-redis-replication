#include <arpa/inet.h>
#include <stdbool.h>
#include <string>
#include <sys/socket.h>

#include "../libs/helpers.h"
#include "../libs/protocol.h"
#include "./transport.hpp"

int main()
{
  int fd = socket(AF_INET, SOCK_STREAM, 0);
  if (fd < 0)
  {
    die("socket()");
  }

  struct sockaddr_in addr = {};
  addr.sin_family = AF_INET;
  addr.sin_port = ntohs(1234);
  addr.sin_addr.s_addr = ntohl(INADDR_LOOPBACK); // 127.0.0.1
  int rv = connect(fd, (const struct sockaddr *)&addr, sizeof(addr));
  if (rv)
  {
    die("connect");
  }

  // multiple pipelined requests
  std::vector<std::string> query_list = {
      "hello1",
      "hello2",
      "hello3",
      // a large message requires multiple event loop iterations
      std::string(K_MAX_MSG, 'z'),
      "hello5",
  };
  for (const std::string &s : query_list)
  {
    bool send_success = transport_send_req(fd, (uint8_t *)s.data(), s.size());
    if (!send_success)
    {
      goto L_DONE;
    }
  }
  for (size_t i = 0; i < query_list.size(); ++i)
  {
    bool read_success = transport_read_res(fd);
    if (!read_success)
    {
      goto L_DONE;
    }
  }

L_DONE:
  close(fd);
  return 0;
}
