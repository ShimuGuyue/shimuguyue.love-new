/**
 * @file handlers.h
 * @brief 登录处理函数 + 路由分发
 */

#pragma once

#include <string>

#include "http/protocol.h"

class ConnectionPool;

namespace http
{

/**
 * @brief 处理密钥登录
 *
 * @details 遍历 users 表，用 Argon2id 验证密钥。
 *
 * @param pool 数据库连接池
 * @param body 请求体 JSON
 * @return JSON 响应字符串
 */
std::string handle_key_login(ConnectionPool& pool, const std::string& body);

/**
 * @brief 处理账号密码登录
 *
 * @details 按 username 索引查找，用 Argon2id 验证密码。
 *
 * @param pool 数据库连接池
 * @param body 请求体 JSON
 * @return JSON 响应字符串
 */
std::string handle_account_login(ConnectionPool& pool, const std::string& body);

/**
 * @brief 路由分发
 *
 * @param pool 数据库连接池
 * @param req  HTTP 请求
 * @return HTTP 响应字符串
 */
std::string route(ConnectionPool& pool, const Request& req);

} // namespace http
