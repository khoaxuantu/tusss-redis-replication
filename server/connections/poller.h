#include "../../libs/io.h"
#include "errno.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#ifndef POLLER_H
#define POLLER_H

/**
 * Fetches a message from the client over the given connection.
 * This function reads the message length, verifies it does not
 * exceed the maximum allowed message size, and then reads the
 * full message into the provided buffer.
 *
 * @param connfd The file descriptor of the client connection.
 * @param read_buffer_ptr A pointer to the buffer where the
 *                        fetched message will be stored.
 *
 * @return true if the message was fetched successfully, false on error
 *         (such as an EOF, read error, or if the message is too long).
 */
static bool poller_fetch(int connfd, char *read_buffer_ptr)
{
  errno = 0;
  bool read_success = io_read(connfd, read_buffer_ptr, MSG_HEADER_SIZE);
  if (!read_success)
  {
    perror(errno == 0 ? "EOF" : "read() error");
    return false;
  }

  uint32_t len = 0;
  memcpy(&len, read_buffer_ptr, MSG_HEADER_SIZE);
  if (len > K_MAX_MSG)
  {
    perror("Too long");
    return false;
  }

  read_success = io_read(connfd, &read_buffer_ptr[MSG_BODY_START_INDEX], len);
  if (!read_success)
  {
    perror("read() error");
    return false;
  }

  return true;
}

/**
 * Sends a response message to the client over the given connection.
 * The message length is prepended to the message in the buffer.
 *
 * @param connfd The file descriptor of the client connection.
 * @param response The response message to send to the client.
 *
 * @return true if the message was sent successfully, false on error.
 */
static bool poller_response(int connfd, const char *response)
{
  uint32_t len = (uint32_t)strlen(response);
  char wbuffer[MSG_HEADER_SIZE + len];

  memcpy(wbuffer, &len, MSG_HEADER_SIZE);
  memcpy(&wbuffer[MSG_BODY_START_INDEX], response, len);

  return io_write(connfd, wbuffer, MSG_HEADER_SIZE + len);
}

/**
 * Handle a client connection. This function reads a message from the
 * client, prints it to stdout, and responds with a message.
 *
 * @param connfd The file descriptor of the client connection.
 *
 * @return true if the connection was handled successfully, false on error.
 */
static bool poller_process(int connfd)
{
  char rbuffer[MSG_SIZE];
  bool read_success = poller_fetch(connfd, rbuffer);
  if (!read_success)
  {
    return false;
  }

  printf("Client says: %s\n", &rbuffer[MSG_BODY_START_INDEX]);

  return poller_response(connfd, "response world");
}

static void poller_polling(int connfd)
{
  while (true)
  {
    if (!poller_process(connfd))
    {
      break;
    }
  }
}

#endif
