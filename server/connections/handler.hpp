#ifndef CONNECTIONS_HANDLER_H
#define CONNECTIONS_HANDLER_H

#include "assert.h"
#include "stdio.h"
#include "string.h"
#include "unistd.h"

#include "../../libs/helpers.h"
#include "../../libs/protocol.h"
#include "./structure.hpp"

static void buf_append(std::vector<uint8_t> *buf, const uint8_t *data, size_t len)
{
  buf->insert(buf->end(), data, data + len);
}


static void buf_remove(std::vector<uint8_t> *buf, size_t len)
{
  buf->erase(buf->begin(), buf->begin() + len);
}

static bool try_a_request(Connection *conn)
{
  // Protocol: message header
  if (conn->incoming.size() < MSG_HEADER_SIZE)
  {
    return false;
  }

  uint32_t len = 0;
  memcpy(&len, conn->incoming.data(), MSG_HEADER_SIZE);

  if (len > K_MAX_MSG)
  {
    conn->want_close = true;
    return false;
  }

  // Protocol: message body
  printf("len: %u\nincoming size: %lu\n", len, conn->incoming.size());
  if (conn->incoming.size() < MSG_HEADER_SIZE + len)
  {
    return false;
  }

  const uint8_t *request = &conn->incoming[MSG_BODY_START_INDEX];

  // TODO: Process the parsed message

  buf_append(&conn->outgoing, (const uint8_t *)&len, MSG_HEADER_SIZE);
  buf_append(&conn->outgoing, request, len);
  buf_remove(&conn->incoming, MSG_HEADER_SIZE + len);

  return true;
}

void handler_read(Connection *conn)
{
  uint8_t buf[64 * 1024];
  ssize_t read_size = read(conn->fd, buf, sizeof(buf));

  if (read_size <= 0)
  {
    conn->want_close = true;
    return;
  }

  buf_append(&conn->incoming, buf, (size_t)read_size);
  try_a_request(conn);

  if (conn->outgoing.size() > 0)
  {
    conn->want_read = false;
    conn->want_write = true;
  }
}

void handler_write(Connection *conn)
{
  assert(conn->outgoing.size() > 0);
  ssize_t rv = write(conn->fd, conn->outgoing.data(), conn->outgoing.size());
  if (rv < 0)
  {
    conn->want_close = true;
    return;
  }

  buf_remove(&conn->outgoing, (size_t)rv);

  if (conn->outgoing.size() == 0)
  {
    conn->want_read = true;
    conn->want_write = false;
  }
}

#endif
