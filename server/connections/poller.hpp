#include "../../libs/protocol.h"
#include "./handler.hpp"
#include "./structure.hpp"
#include "errno.h"
#include "stdbool.h"
#include "stdint.h"
#include "stdio.h"
#include "string.h"

#include "arpa/inet.h"
#include "fcntl.h"
#include "poll.h"
#include "sys/socket.h"

#include <vector>

#ifndef CONNECTIONS_POLLER_H
#define CONNECTIONS_POLLER_H

static void _prepare(int fd,
                     std::vector<Connection *> *fd2conn,
                     std::vector<struct pollfd> *poll_args) noexcept
{
  poll_args->clear();
  struct pollfd pfd = {fd, events : POLLIN, revents : 0};
  poll_args->push_back(pfd);

  for (Connection *conn : (*fd2conn))
  {
    if (!conn)
    {
      continue;
    }

    struct pollfd pfd = {fd : conn->fd, events : POLLERR, revents : 0};

    if (conn->want_read)
    {
      pfd.events |= POLLIN;
    }

    if (conn->want_write)
    {
      pfd.events |= POLLOUT;
    }

    poll_args->push_back(pfd);
  }
}

static int _polling(std::vector<struct pollfd> *poll_args)
{
  int rv = poll(poll_args->data(), (nfds_t)poll_args->size(), -1);

  if (rv < 0 && errno == EINTR)
  {
    return -2; // not an error to break
  }

  if (rv < 0)
  {
    die("poll()");
  }

  return rv;
}

static void _fd_set_nb(int fd)
{
  fcntl(fd, F_SETFL, fcntl(fd, F_GETFL, 0) | O_NONBLOCK);
}

static Connection *_accept_new_conn(int fd)
{
  struct sockaddr_in client_addr = {};
  socklen_t addr_len = sizeof(client_addr);
  int connfd = accept(fd, (struct sockaddr *)&client_addr, &addr_len);
  if (connfd < 0)
  {
    return nullptr;
  }

  _fd_set_nb(connfd);

  Connection *conn = new Connection();
  conn->fd = connfd;
  conn->want_read = true;

  return conn;
}

static void _setup_listening_socket(int fd,
                                    std::vector<Connection *> *fd2conn,
                                    std::vector<struct pollfd> *poll_args)
{
  if (poll_args->at(0).revents)
  {
    if (Connection *conn = _accept_new_conn(fd))
    {
      if (fd2conn->size() <= (size_t)conn->fd)
      {
        fd2conn->resize(conn->fd + 1);
      }

      fd2conn->at(conn->fd) = conn;
    }
  }
}

static void _handle_connection(std::vector<Connection *> *fd2conn,
                               std::vector<struct pollfd> *poll_args)
{
  for (size_t i = 1; i < poll_args->size(); i++)
  {
    uint32_t ready = poll_args->at(i).revents;
    Connection *conn = fd2conn->at(poll_args->at(i).fd);

    if (ready & POLLIN)
    {
      handler_read(conn);
    }

    if (ready & POLLOUT)
    {
      handler_write(conn);
    }

    if ((ready & POLLERR) || conn->want_close)
    {
      close(conn->fd);
      fd2conn->at(conn->fd) = nullptr;
      delete conn;
    }
  }
}

void poller_poll(int fd)
{
  std::vector<Connection *> fd2conn;
  std::vector<struct pollfd> poll_args;

  while (true)
  {
    _prepare(fd, &fd2conn, &poll_args);

    int rv = _polling(&poll_args);
    if (rv == -2)
    {
      continue;
    }

    _setup_listening_socket(fd, &fd2conn, &poll_args);
    _handle_connection(&fd2conn, &poll_args);
  }
}

#endif
