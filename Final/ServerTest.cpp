#include "TCPServer.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//Pre: fd is a valid file descritor of a client that has sent data to the server
//Post: return exact data back to client
bool echo_handler(int fd)
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
    return true;
}

int main()
{
    TCPServer server(4);

    server.listen_and_serve(8080, echo_handler);

    return 0;
}