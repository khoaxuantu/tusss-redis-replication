#ifndef TRANSPORT_HPP
#define TRANSPORT_HPP

#include <errno.h>
#include <stdbool.h>
#include <stdint.h>
#include <vector>
#include <string.h>

#include "../libs/protocol.h"
#include "io.h"

static void _buf_append(std::vector<uint8_t> &buf, const uint8_t *data, size_t len)
{
  buf.insert(buf.end(), data, data + len);
}

bool transport_send_req(int fd, const uint8_t *text, size_t len)
{
  if (len > K_MAX_MSG)
  {
    return false;
  }

  std::vector<uint8_t> wbuf;
  _buf_append(wbuf, (const uint8_t *)&len, MSG_HEADER_SIZE);
  _buf_append(wbuf, text, len);
  return io_write(fd, (const char *)wbuf.data(), wbuf.size());
}

bool transport_read_res(int fd)
{
  std::vector<uint8_t> rbuf;
  rbuf.resize(MSG_HEADER_SIZE);

  errno = 0;

  bool read_success = io_read(fd, (char *)rbuf.data(), MSG_HEADER_SIZE);
  if (!read_success)
  {
    if (errno == 0)
    {
      printf("EOF\n");
    }
    else
    {
      printf("read() error\n");
    }
    return false;
  }

  uint32_t len = 0;
  memcpy(&len, rbuf.data(), MSG_HEADER_SIZE);
  if (len > K_MAX_MSG)
  {
    printf("Too long\n");
    return false;
  }

  rbuf.resize(MSG_HEADER_SIZE + len);
  read_success = io_read(fd, (char *)&rbuf[4], len);
  if (!read_success)
  {
    if (errno == 0)
    {
      printf("EOF\n");
    }
    else
    {
      printf("read() error\n");
    }
    return false;
  }

  printf("len:%u data:%.*s\n", len, len < 100 ? len : 100, &rbuf[4]);
  return true;
}

#endif
