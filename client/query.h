#include "../libs/io.h"
#include "errno.h"
#include "stdbool.h"
#include "stdint.h"
#include "string.h"

#ifndef QUERY_H
#define QUERY_H

static bool query(int fd, const char *text)
{
  uint32_t len = (uint32_t)strlen(text);
  if (len > K_MAX_MSG)
  {
    printf("Too long");
    return false;
  }

  char wbuffer[MSG_SIZE];
  memcpy(wbuffer, &len, MSG_HEADER_SIZE);
  memcpy(&wbuffer[MSG_BODY_START_INDEX], text, len);
  if (!io_write(fd, wbuffer, MSG_HEADER_SIZE + len))
  {
    printf("write() error");
    return false;
  }

  char rbuffer[MSG_SIZE + 1];
  errno = 0;
  if (!io_read(fd, rbuffer, MSG_HEADER_SIZE))
  {
    printf(errno == 0 ? "EOF" : "read() error");
    return false;
  }

  memcpy(&len, rbuffer, MSG_HEADER_SIZE);
  if (len > K_MAX_MSG)
  {
    printf("Too long");
    return false;
  }

  if (!io_read(fd, &rbuffer[MSG_BODY_START_INDEX], len))
  {
    printf("read() error");
    return false;
  }

  printf("Server says: %.*s\n", len, &rbuffer[MSG_BODY_START_INDEX]);
  return true;
}

#endif
