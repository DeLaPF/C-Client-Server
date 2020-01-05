#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <functional>
#include <iostream>

class ThreadPool; // Forward declare class dependencies in header file
class SyncLinkedList;
class LinkedList;

class TCPServer {
public:
    // Constructor
    TCPServer(size_t max_concurrency);
    // Destructor
    ~TCPServer();
    
    // This is a blocking call that listens for connections on the given port and
    // serves them using the provided handler. The handler should return true
    // if the connection should be closed after serving and false otherwise.
    void listen_and_serve(size_t port, std::function<bool (int)> handler);
private:
    const int BACKLOG_SIZE = 2;
    int sock_fd;
    static SyncLinkedList *open_conns;
    ThreadPool *thread_pool;

    void handle_connection(fd_set read_fds, std::function<bool (int)> handler);
    void accept_connection();
    void error(const char *msg);
};

#endif