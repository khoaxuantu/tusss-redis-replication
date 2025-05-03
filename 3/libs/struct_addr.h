#include "stdio.h"
#include "stdint.h"

#if !defined(STRUCT_ADDR_H)
#define STRUCT_ADDR_H

struct InAddr
{
  uint32_t s_addr;
};

struct SockAddrIn
{
  uint16_t sin_family;
  uint16_t sin_port;
  struct InAddr sin_addr;
};

void print_struct_addr(struct SockAddrIn* addr) {
  printf("SockAddrIn\n");
  printf("sin_family: %d\n", addr->sin_family);
  printf("sin_port: %d\n", addr->sin_port);
  printf("sin_addr: { s_addr: %d }\n", addr->sin_addr.s_addr);
  printf("Size: %ld\n\n", sizeof(*addr));
}

#endif
