/**
 * @file server.h
 * @brief HTTP 服务器
 */

#pragma once

class ConnectionPool;

namespace http
{

class Server
{
private:
    int port_;
    int server_fd_;
    ConnectionPool* pool_;

public:
    /**
     * @brief 构造函数 — 仅保存参数，不建立连接
     *
     * @param port 监听端口
     * @param pool 数据库连接池指针（调用方保证生命周期）
     */
    explicit Server(int port, ConnectionPool* pool);

    ~Server();

    Server(const Server&) = delete;
    Server& operator=(const Server&) = delete;

public:
    /**
     * @brief 启动服务器（创建 socket、绑定、监听）
     * @return true 成功
     */
    bool start();

    /**
     * @brief 运行事件循环（阻塞）
     */
    void run();

    /**
     * @brief 停止服务器
     */
    void stop();
};

} // namespace http
