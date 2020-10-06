#include "tcp_server.h"

void error(const char *msg)
{
    perror(msg);
    exit(0);
}

//Pre: fd is a valid file descritor of a client that has sent data to the server
//Post: return exact data back to client
bool echo_handler(int fd)
{
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

//Pre: fd is a valid file descritor of a client that has sent an http request to the server
//Post: display html to client's browser in accordance with the client's request
bool http_handler(int fd)
{
    char buffer[256]; // create buffer of 256 chars
    bzero(buffer, 256); // zero out all values in buffer
    int recvd = recv(fd, buffer, 255, 0); // receive data from fd and put it in buffer
    if (recvd < 0)
        error("ERROR data not received");
    std::string resp = "HTTP/1.1 200 OK\r\nContent-Length: 48\r\nContent-Type: text/html\r\nConnection: Closed\r\n\r\n<html>\r\n<body>\r\n<h1>Hello, World!</h1>\r\n</body>\r\n</html>";
    //std::string resp = "HTTP/1.1 200 OK\nContent-Type: text/plain\nContent-Length: 12\n\nHello World!";
    for (char c : buffer)
        std::cout << c;
    int sent = send(fd, resp.c_str(), resp.length(), 0);
    if (sent < 0)
        error("ERROR response not sent");
    return true;
}

int main()
{
    dlpf::net::tcp::tcp_server tcp_server(4);

    tcp_server.listen_and_serve(8080, http_handler);

    return 0;
}