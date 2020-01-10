#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "thread_pool.h"
#include "SyncLinkedList.h"

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <functional>
#include <iostream>

// Forward declarations
namespace dlpf {
  namespace sync {
    class linked_list;
  } // dlpf::sync

  namespace async {
    class thread_pool;
  } // dlpf::async
} // dlpf


namespace dlpf::net::tcp {
  class server {
    public:
      // Constructor
      server(size_t max_concurrency):thread_pool(max_concurrency), open_conns(){};
      // Destructor
      ~server();

      // This is a blocking call that listens for connections on the given port and
      // serves them using the provided handler. The handler should return true
      // if the connection should be closed after serving and false otherwise.
      void listen_and_serve(size_t port, std::function<bool (int)> handler);
    private:
      const int BACKLOG_SIZE = 2;
      int sock_fd;
      SyncLinkedList open_conns;
      dlpf::async::thread_pool thread_pool;

      void accept_connection();
      void handle_connection(fd_set read_fds, std::function<bool (int)> handler);
      void error(const char *msg);
  };
} // dlpf::net::tcp

#endif