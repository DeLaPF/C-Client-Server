#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "tcp_server.h"
#include <string.h>

namespace dlpf::http
{

    typedef struct HTTPRequest
    {
        std::string method;
        std::string path;
    } HTTPRequest;

    typedef struct HTTPResponse
    {
        int status_code;
        std::string status_message;
        std::unordered_map<std::string, std::string> headers;
        std::string body;
    } HTTPResponse;

    class http_server
    {
    public:
        http_server(size_t max_concurrency): tcp_server(max_concurrency) {};
        void registerHandler(std::string route, std::function<const HTTPResponse& (const HTTPRequest &)> handler);
        void listen_and_serve(size_t port);
    private:
        dlpf::net::tcp::tcp_server tcp_server;
        std::unordered_map<std::string, std::function<const HTTPResponse& (const HTTPRequest &)>> handlers;
        bool http_handler(int fd);
        HTTPRequest parse_http_request(int fd);
        void write_response(int fd, const HTTPResponse &resp);
        void write_error(int fd);
    };
} // dlpf::http
#endif