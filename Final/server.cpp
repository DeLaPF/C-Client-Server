#include "server.h"

dlpf::net::tcp::server::~server()
{
    close(sock_fd);
}

void dlpf::net::tcp::server::listen_and_serve(size_t port, std::function<bool (int)> handler)
{
    // Step 1: Set up socket
    sock_fd = socket(AF_INET, SOCK_STREAM, 0); // Create socket file descriptor for server
    if (sock_fd < 0)
        error("ERROR opening socket");

    // Step 2: Bind
    struct sockaddr_in host;
    host.sin_family = AF_INET; // IPv4 connection
    host.sin_port = htons(port); // bound to port 8080
    host.sin_addr.s_addr = INADDR_ANY; // connected to localhost
    const struct sockaddr *host_ptr = (struct sockaddr *) &host;
    // bind sock_fd to port
    if (bind(sock_fd, host_ptr, sizeof(host)) < 0)
        error("ERROR binding to socket");

    // Step 3: Listen
    if (listen(sock_fd, BACKLOG_SIZE) < 0) // The socket will actively listen for incoming connections and queue them up
        error("ERROR listener failed");

    while (true)
    {
        // Reset read_fds
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(sock_fd, &read_fds);
        int maxfd = 0;
        std::function<bool (dlpf::sync::node *)> iter_full = [&read_fds, &maxfd] (dlpf::sync::node *cur) {
            int data = cur->data;
            FD_SET(data, &read_fds); // populate read_fds from open_conns
            if (data > maxfd) // calculate maxfd
                maxfd = data;
            return true;
        };
        open_conns.for_each(iter_full); // iterate through entire list

        if (sock_fd > maxfd)
            maxfd = sock_fd;

        std::cout << "Attempting to Select . . ." << std::endl;
        if (select(maxfd + 1, &read_fds, NULL, NULL, NULL) < 0) // sleeps until there is an available connection and then determines if it must be accepted or read from
            error("ERROR Selection Failed");
        std::cout << "Connection Selected" << std::endl;

        if (FD_ISSET(sock_fd, &read_fds))
            accept_connection();
        else
            handle_connection(read_fds, handler);
    }
}

//Pre: sock_fd is the socket bound to this server
//Post: accept connection and add file descriptor of connected client to open_conns
void dlpf::net::tcp::server::accept_connection()
{
    // Step 4: Accecpt
    struct sockaddr_in remote_host; // a pointer to a sockaddr struct that will be filled in with the address information of the remote host
    socklen_t remote_len = sizeof(remote_host); // the size of the sockaddr struct in bytes
    std::cout << "Attempting to Accept . . ." << std::endl;
    int remotefd = accept(sock_fd, (struct sockaddr *) &remote_host, &remote_len); // dequeue from sock_fd and accept connection through remotefd
    if (remotefd < 0)
        error("ERROR accepting request");
    std::cout << "Connection Accecpted" << std::endl;
    open_conns.push(remotefd);
}

//Pre: open_conns contains fd that is also in with read_fds
//Post: determine which fd has been updated and echo data
void dlpf::net::tcp::server::handle_connection(fd_set read_fds, std::function<bool (int)> handler)
{
    int fd = -1;
    std::function<bool (dlpf::sync::node *)> iter_search = [read_fds, &fd] (dlpf::sync::node *cur){
        if (FD_ISSET(cur->data, &read_fds)) { // check if file descriptor is ready to be read from
            fd = cur->data;
            return false;
        }
        return true;
    };
    open_conns.for_each(iter_search);  // iterate through fdlist

    if(fd < 0)
        error("ERROR Connection not in list");

    std::cout << "Handeling Received Data" << std::endl;

    open_conns.remove(fd); //avoid race conditon of fd being checked against twice if handler is a slow enough funtion
    thread_pool.enqueue([fd, handler, this]
    {
        if(handler(fd))
            close(fd);
        else
            open_conns.push(fd); // re-add to continue connection
    });
}

void dlpf::net::tcp::server::error(const char *msg)
{
    perror(msg);
    exit(0);
}