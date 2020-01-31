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
    const HTTPResponse &resp = handler.second(req);

    // Write the response to the connection
    write_response(fd, resp);

    return true;
}

void dlpf::http::http_server::listen_and_serve(size_t port)
{
    tcp_server.listen_and_serve(port, dlpf::http::http_server::http_handler);
}

dlpf::http::HTTPRequest dlpf::http::http_server::parse_http_request(int fd)
{
    struct HTTPResponse
    {
        status_code = 200,
        status_message = "OK",
        headers = {{"Content-Type", "plain/text"}},
        body = "Hello world!",
    } response;

    return response;
}

void dlpf::http::http_server::write_response(int fd, const HTTPResponse &resp)
{

}

void dlpf::http::http_server::write_error(int fd)
{

}