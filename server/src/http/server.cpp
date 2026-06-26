/**
 * @file server.cpp
 * @brief HTTP 服务器实现
 */

#include "http/server.h"
#include "http/protocol.h"
#include "http/handlers.h"

#include <iostream>

#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

namespace http
{

Server::Server(int port, ConnectionPool* pool)
    : port_(port)
    , server_fd_(-1)
    , pool_(pool)
{ }

Server::~Server()
{
    stop();
}

bool Server::start()
{
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ < 0)
    {
        std::cerr << "[error] 创建 socket 失败\n";
        return false;
    }

    int opt{ 1 };
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    sockaddr_in addr{};
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(static_cast<uint16_t>(port_));

    if (bind(server_fd_, reinterpret_cast<sockaddr*>(&addr), sizeof(addr)) < 0)
    {
        std::cerr << "[error] 绑定端口 " << port_ << " 失败\n";
        close(server_fd_);
        server_fd_ = -1;
        return false;
    }

    if (listen(server_fd_, 10) < 0)
    {
        std::cerr << "[error] 监听失败\n";
        close(server_fd_);
        server_fd_ = -1;
        return false;
    }

    std::cout << "[ok] HTTP 服务器已启动，监听 http://localhost:" << port_ << "\n";
    return true;
}

void Server::run()
{
    while (server_fd_ >= 0)
    {
        sockaddr_in client_addr{};
        socklen_t client_len = sizeof(client_addr);
        int client_fd = accept(
            server_fd_,
            reinterpret_cast<sockaddr*>(&client_addr),
            &client_len);

        if (client_fd < 0)
        {
            std::cerr << "[warn] accept 失败\n";
            continue;
        }

        Request req = parse_request(client_fd);
        if (req.path.empty())
        {
            std::string resp = build_response(
                400, R"({"success":false,"message":"无效请求"})");
            send(client_fd, resp.data(), resp.size(), 0);
            close(client_fd);
            continue;
        }

        std::cout << "[req] " << req.method << " " << req.path << '\n';

        std::string response = route(*pool_, req);
        send(client_fd, response.data(), response.size(), 0);
        close(client_fd);
    }
}

void Server::stop()
{
    if (server_fd_ >= 0)
    {
        close(server_fd_);
        server_fd_ = -1;
    }
}

} // namespace http
