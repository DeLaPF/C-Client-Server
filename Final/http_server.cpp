#include "http_server.h"

void dlpf::http::http_server::registerHandler(std::string route, std::function<const HTTPResponse& (const HTTPRequest &)> handler)
{
    handlers[route] = handler;
}

bool dlpf::http::http_server::http_handler(int fd)
{
    // Parse request
    const HTTPRequest &req = parse_http_request(fd);

    // Find the appropriate handler for the requested path
    auto &handler = handlers.find(req.path);

    // If the handler does not exist, write a generic error response and close the connection
    if (handler == std::unordered_map::end)
    {
        write_error(fd);
        return true;
    }

    // Else call the handler
    const HTTPResponse &resp = handler(req);

    // Write the response to the connection
    write_response(fd, resp);

    return true;
}

void dlpf::http::http_server::listen_and_serve(size_t port)
{
    tcp_server.listen_and_serve(port, http_handler);
}

dlpf::http::HTTPRequest dlpf::http::http_server::parse_http_request(int fd)
{
    char buffer[256]; // create buffer of 256 chars
    bzero(buffer, 256); // zero out all values in buffer
    int recvd = recv(fd, buffer, 255, 0); // receive data from fd and put it in buffer
    if (recvd < 0)
        error("ERROR data not received");
    std::string req = buffer;
    HTTPRequest request;
    request.method = req.substr(0, req.find(' '));
    request.path = req.substr(req.find('/'), req.find(' '));

    return request;
}

void dlpf::http::http_server::write_response(int fd, const HTTPResponse &resp)
{
    std::string response = resp.status_code + "\r\n";
    response += resp.status_message + "\r\n";
    for (std::pair<std::string, std::string> header : resp.headers)
    {
        response += header.first + ":" + header.second + "\r\n";
    }
    response += "\r\n\r\n" + resp.body;
    int sent = send(fd, response.c_str(), response.length(), 0);
    if (sent < 0)
        error("ERROR data not sent");
}

void dlpf::http::http_server::write_error(int fd)
{
    std::string error = "HTTP/1.1 404 Not Found\nContent-Type: text/plain\nContent-Length: 13\n\n404 Not Found";
    int sent = send(fd, error.c_str(), error.length(), 0);
    if (sent < 0)
        error("ERROR error not sent");
}

void dlpf::http::http_server::error(const char *msg)
{
    perror(msg);
    exit(0);
}