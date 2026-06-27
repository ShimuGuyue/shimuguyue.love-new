/**
 * @file main.cpp
 * @brief 服务入口
 */

#include <iostream>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>

#include "config/config.h"
#include "db/connection_pool.h"
#include "http/server.h"

namespace
{

/** 递归创建目录，失败时通过 err 返回原因 */
bool mkdir_p(const std::string& path, std::string& err)
{
    std::string cur;
    for (std::size_t i = 0; i < path.size(); ++i)
    {
        cur.push_back(path[i]);
        if (path[i] == '/' || i == path.size() - 1)
        {
            if (!cur.empty() && cur != "/")
            {
                struct stat st;
                if (stat(cur.c_str(), &st) != 0)
                {
                    if (mkdir(cur.c_str(), 0755) != 0)
                    {
                        err = "mkdir(" + cur + "): " + strerror(errno);
                        return false;
                    }
                }
                else if (!S_ISDIR(st.st_mode))
                {
                    err = cur + " 已存在但不是目录";
                    return false;
                }
            }
        }
    }
    return true;
}

} // namespace

int main()
{
    // 校验 BLOG_ROOT
    std::cout << "[init] 正在初始化博客路径...\n";
    auto blog_root = config::blog_root();
    if (!blog_root)
    {
        std::cerr << "[error] 环境变量 BLOG_ROOT 未设置\n";
        return 1;
    }

    std::string err;
    if (!mkdir_p(*blog_root, err))
    {
        std::cerr << "[error] 无法创建博客根目录\n"
                  << err << "\n";
        return 1;
    }
    std::cout << "[ok] 博客路径初始化成功\n";

    // 连接数据库
    std::cout << "[init] 正在连接数据库...\n";
    ConnectionPool pool{ config::db_connection_string() };
    if (!pool.connect())
    {
        std::cerr << "[error] 数据库连接失败: " << pool.last_error() << '\n';
        return 1;
    }
    std::cout << "[ok] 数据库连接成功\n";

    // 启动 HTTP 服务
    http::Server server{ 8080, &pool };
    if (!server.start())
        return 1;

    server.run();
    return 0;
}