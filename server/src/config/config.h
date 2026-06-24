/**
 * @file config.h
 * @brief 数据库配置
 *
 * @details
 * 连接参数完全由 libpq 原生配置文件管理
 * - @c ~/.pg_service.conf：服务定义（dbname、user、host、port）
 * - @c ~/.pgpass：密码
 */

#pragma once
#include <string>

namespace config
{

/**
 * @brief 获取 PostgreSQL 连接字符串
 *
 * 引用 @c ~/.pg_service.conf 中定义的 @c [love] 节。
 *
 * @see https://www.postgresql.org/docs/current/libpq-pgservice.html
 */
std::string db_connection_string()
{
    return "service=love";
}

/**
 * @brief 获取连接池大小
 *
 * @return 固定值 5
 */
int pool_size()
{
    return 5;
}

} // namespace config
