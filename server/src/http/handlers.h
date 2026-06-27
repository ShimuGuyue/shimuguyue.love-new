/**
 * @file handlers.h
 * @brief HTTP 路由分发
 */

#pragma once

#include <string>

#include "http/protocol.h"

class ConnectionPool;

namespace http
{

/**
 * @brief 路由分发
 *
 * @details 根据请求方法和路径，派发到对应的处理函数。
 *
 * 路由表：
 * | 方法 | 路径                  | 处理函数                 |
 * |------|----------------------|-------------------------|
 * | POST | /api/login/key       | handle_key_login        |
 * | POST | /api/login/account   | handle_account_login    |
 * | POST | /api/blogs/import    | handle_blog_import      |
 *
 * @param pool 数据库连接池
 * @param req  HTTP 请求
 * @return HTTP 响应字符串
 */
std::string route(ConnectionPool& pool, const Request& req);

} // namespace http