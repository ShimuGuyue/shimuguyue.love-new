/**
 * @file main.cpp
 * @brief 服务入口 — 初始化数据库连接
 */

#include <iostream>

#include "config/config.h"
#include "db/connection_pool.h"

int main()
{
    std::cout << "[init] 正在连接数据库...\n";
    ConnectionPool pool{ config::db_connection_string() };
    if (!pool.connect())
    {
        std::cerr << "[error] 数据库连接失败: " << pool.last_error() << '\n';
        return 1;
    }
    std::cout << "[ok] 数据库连接成功\n";

    // TODO: 启动 HTTP 服务器，进入事件循环
}
