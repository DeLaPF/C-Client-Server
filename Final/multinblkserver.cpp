#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <iostream>
#include "ThreadPool.h"
#include "LinkedList.h"

const int PORT_NO = 8080;
const int BACKLOG_SIZE = 2;

// Prototypes
void handle_connection(fd_set read_fds, LinkedList *open_conn_fds, ThreadPool *thread_pool);
void echo(int fd);
void accept_connection(int sockfd, LinkedList *open_conn_fds);

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

int main(int argc, char const *argv[])
{
    // Step 1: Set up socket
    int sockfd = socket(AF_INET, SOCK_STREAM, 0); // Create socket file descriptor for server
    if (sockfd < 0)
        error("ERROR opening socket");

    // Step 2: Bind
    struct sockaddr_in host;
    host.sin_family = AF_INET; // IPv4 connection
    host.sin_port = htons(PORT_NO); // bound to port 8080
    host.sin_addr.s_addr = INADDR_ANY; // connected to localhost
    const struct sockaddr *host_ptr = (struct sockaddr *) &host;
    int bound = bind(sockfd, host_ptr, sizeof(host)); // bind sockfd to port
    if (bound < 0)
        error("ERROR binding to socket");

    // Step 3: Listen
    int listening = listen(sockfd, BACKLOG_SIZE); // The socket will actively listen for incoming connections and queue them up
    if (listening < 0)
        error("ERROR listener failed");

    ThreadPool thread_pool(4); // create threadpool of size 4
    LinkedList *open_conn_fds = new LinkedList();
    while (1)
    {
        // Reset read_fds
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sockfd, &read_fds);
        int maxfd = 0;
        for (node *open_conn_fd = open_conn_fds->begin(); open_conn_fd; open_conn_fd = open_conn_fd->next)
        {
            int data = open_conn_fd->data;
            FD_SET(data, &read_fds); // populate read_fds from open_conn_fds
            if (data > maxfd) // calculate maxfd
                maxfd = data;
        }

        if (sockfd > maxfd)
            maxfd = sockfd;

        std::cout << "Attempting to Select . . ." << std::endl;
        if (select(maxfd + 1, &read_fds, NULL, NULL, NULL) < 0) // sleeps until there is an available connection and then determines if it must be accepted or read from
            error("My error message");
        std::cout << "Connection Selected" << std::endl;

        if (FD_ISSET(sockfd, &read_fds))
            accept_connection(sockfd, open_conn_fds);
        else
            handle_connection(read_fds, open_conn_fds, &thread_pool);
    }

    // Close the connection
    std::cout << "Closing the connection" << std::endl;
    close(sockfd);
    return 0;
}

//Pre: open_conn_fds contains fd that is also in with read_fds
//Post: determine which fd has been updated and echo data
void handle_connection(fd_set read_fds, LinkedList *open_conn_fds, ThreadPool *thread_pool)
{
    int fd;
    for (node *open_conn_fd = open_conn_fds->begin(); open_conn_fd; open_conn_fd = open_conn_fd->next)  // iterate through fdlist
    {
        fd = open_conn_fd->data;
        if (FD_ISSET(fd, &read_fds))   // check if file descriptor is ready to be read from
            break;
    }
    std::cout << "Echoing Data" << std::endl;
    open_conn_fds->remove(fd);

    thread_pool->enqueue([fd] {
    	echo(fd); // echo data from fd (section to change for different server functionality)
    });
}

//Pre: sockfd is the socket bound to this server
//Post: accept connection and add file descriptor of connected client to open_conn_fds
void accept_connection(int sockfd, LinkedList *open_conn_fds)
{
    // Step 4: Accecpt
    struct sockaddr_in remote_host; // a pointer to a sockaddr struct that will be filled in with the address information of the remote host
    socklen_t remote_len = sizeof(remote_host); // the size of the sockaddr struct in bytes
    std::cout << "Attempting to Accept . . ." << std::endl;
    int remotefd = accept(sockfd, (struct sockaddr *) &remote_host, &remote_len); // dequeue from sockfd and accept connection through remotefd
    if (remotefd < 0)
        error("ERROR accepting request");
    std::cout << "Connection Accecpted" << std::endl;
    open_conn_fds->push(remotefd);
}

//Pre: fd is a valid file descritor of a client that has sent data to the server
//Post: return exact data back to client
void echo(int fd)
{
    // Step 5: Echo
    char buffer[256]; // create buffer of 256 chars
    bzero(buffer, 256); // zero out all values in buffer
    int recvd = recv(fd, buffer, 255, 0); // receive data from fd and put it in buffer
    if (recvd < 0)
        error("ERROR data not received");
    int sent = send(fd, buffer, strlen(buffer), 0); // send buffer back to echo data
    if (sent < 0)
        error("ERROR data not sent");
    close(fd); // close connection to client part 3
}