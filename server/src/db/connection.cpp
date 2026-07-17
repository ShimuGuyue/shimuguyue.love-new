/**
 * @file db/connection.cpp
 * @brief 数据库连接管理函数的实现
 */

#include "db/connection.h"

#include <cstdlib>
#include <format>
#include <iostream>
#include <string>

namespace {

/**
 * @brief 读取必需的环境变量，未设置时打印错误信息并退出程序。
 * @param key 环境变量名。
 * @return 环境变量的值。
 */
[[nodiscard]]auto env_required(const char* key) -> std::string
{
    const char* val{ std::getenv(key) };
    if (val == nullptr)
    {
        std::cerr << std::format("错误：缺少必需的环境变量 {}。\n", key);
        std::exit(1);
    }
    return val;
}

} // namespace



// ================================================== //



namespace db {

auto connect() -> pqxx::connection
{
    /*log*/std::cout << "正在连接至数据库...\n";

    auto host     = env_required("PGHOST");
    auto port     = env_required("PGPORT");
    auto dbname   = env_required("PGDATABASE");
    auto user     = env_required("PGUSER");
    auto password = env_required("PGPASSWORD");

    auto conn_str = std::format(
        "host={} port={} dbname={} user={} password={}",
        host, port, dbname, user, password
    );
    pqxx::connection conn{ conn_str };

    if (!conn.is_open())
    {
        /*log*/std::cout << "连接至数据库失败！\n";
        exit(1);
    }
    /*log*/std::cout << std::format("成功连接至数据库！\n");
    return conn;
}

} // namespace db
