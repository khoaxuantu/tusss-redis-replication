#ifndef CONNECTIONS_STRUCTURE_H
#define CONNECTIONS_STRUCTURE_H

#include <vector>
#include <stdint.h>

struct Connection
{
  int fd = -1;
  bool want_read = false;
  bool want_write = false;
  bool want_close = false;

  std::vector<uint8_t> incoming;
  std::vector<uint8_t> outgoing;
};

#endif
