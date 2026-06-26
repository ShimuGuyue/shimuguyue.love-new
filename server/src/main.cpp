/**
 * @file main.cpp
 * @brief 服务入口
 */

#include <iostream>

#include "config/config.h"
#include "db/connection_pool.h"
#include "http/server.h"

int main()
{
    std::cout << "[init] 正在初始化博客路径...\n";
    auto blog_root = config::blog_root();
    if (!blog_root)
    {
        std::cerr << "[error] 环境变量 BLOG_ROOT 未设置\n"
                  << "请执行 export BLOG_ROOT=xxxxxx 后重试\n";
        return 1;
    }
    std::cout << "[ok] 博客路径初始化成功\n";

    std::cout << "[init] 正在连接数据库...\n";
    ConnectionPool pool{ config::db_connection_string() };
    if (!pool.connect())
    {
        std::cerr << "[error] 数据库连接失败: " << pool.last_error() << '\n';
        return 1;
    }
    std::cout << "[ok] 数据库连接成功\n";

    http::Server server{ 8080, &pool };
    if (!server.start())
        return 1;

    server.run();
    return 0;
}