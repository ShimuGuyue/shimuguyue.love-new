/**
 * @file db/connection.h
 * @brief 数据库连接管理函数的定义
 */
#pragma once

#include <pqxx/pqxx>

namespace db {

/**
 * @brief 从环境变量读取数据库连接参数，建立并返回一个 pqxx::connection。
 *
 * 依次读取 PGHOST、PGPORT、PGDATABASE、PGUSER、PGPASSWORD 环境变量，
 * 若未设置则打印缺失信息并中断程序。
 *
 * @return 已打开的 pqxx::connection。
 */
auto connect() -> pqxx::connection;

} // namespace db
