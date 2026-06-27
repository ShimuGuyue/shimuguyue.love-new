/**
 * @file login_handler.h
 * @brief 登录认证接口
 *
 * @details 支持两种登录方式：
 * - 密钥登录：遍历 users 表，用 Argon2id 验证密钥哈希
 * - 账号登录：按 username 索引查找，用 Argon2id 验证密码哈希
 *
 * 登录成功后返回用户信息（id, username）及权限列表（permissions）。
 */

#pragma once

#include <string>

class ConnectionPool;

namespace http
{

/**
 * @brief 处理密钥登录
 *
 * @details
 * 请求体：@c {"key":"..."}  \n
 * 遍历全部用户，用 libsodium Argon2id 逐一比对密钥哈希。\n
 * 成功返回 200 + 用户信息和权限；失败返回 401。
 *
 * @param pool 数据库连接池
 * @param body 请求体 JSON
 * @return JSON 响应字符串
 */
std::string handle_key_login(ConnectionPool& pool, const std::string& body);

/**
 * @brief 处理账号密码登录
 *
 * @details
 * 请求体：@c {"username":"...","password":"..."}  \n
 * 按 username 唯一索引查找用户，用 Argon2id 验证密码哈希。\n
 * 成功返回 200 + 用户信息和权限；用户名不存在或密码错误返回 401。
 *
 * @param pool 数据库连接池
 * @param body 请求体 JSON
 * @return JSON 响应字符串
 */
std::string handle_account_login(ConnectionPool& pool, const std::string& body);

} // namespace http