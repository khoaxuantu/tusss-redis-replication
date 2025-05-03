#ifndef IO_H
#define IO_H

#include "assert.h"
#include "stdio.h"
#include "unistd.h"
#include "stdbool.h"

const size_t K_MAX_MSG = 4096;
const size_t MSG_HEADER_SIZE = 4; // bytes
const size_t MSG_BODY_START_INDEX = 4;
const size_t MSG_SIZE = MSG_HEADER_SIZE + K_MAX_MSG;

/**
 * Reads data from the specified file descriptor into the provided buffer.
 * Continues reading until the requested number of bytes is read or an
 * error occurs. If an error occurs or EOF is encountered, the function
 * returns false; otherwise, it returns true after all data has been read
 * successfully.
 *
 * @param fd The file descriptor from which data will be read.
 * @param buffer A pointer to the buffer where the read data will be stored.
 * @param n The number of bytes to read into the buffer.
 *
 * @return true if all data is read successfully, false on error.
 */
static bool io_read(int fd, char *buffer, size_t n)
{
  while (n > 0)
  {
    ssize_t rv = read(fd, buffer, n);
    if (rv <= 0)
    {
      return false;
    }

    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buffer += rv;
  }
  return true;
}

/**
 * Writes data from the buffer to the specified file descriptor until
 * the entire buffer is written or an error occurs. If an error occurs
 * or EOF is encountered, the function returns false; otherwise, it
 * returns true after all data has been written successfully.
 *
 * @param fd The file descriptor to which data will be written.
 * @param buffer A pointer to the buffer containing the data to be written.
 * @param n The number of bytes to write from the buffer.
 *
 * @return true if all data is written successfully, false on error.
 */
static bool io_write(int fd, const char *buffer, size_t n)
{
  while (n > 0)
  {
    ssize_t rv = write(fd, buffer, n);
    if (rv <= 0)
    {
      return false;
    }

    assert((size_t)rv <= n);
    n -= (size_t)rv;
    buffer += rv;
  }
  return true;
}

#endif
