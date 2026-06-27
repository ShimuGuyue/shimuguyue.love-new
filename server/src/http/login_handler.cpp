/**
 * @file login_handler.cpp
 * @brief 登录认证实现
 *
 * @details
 * 认证流程：
 * 1. 从请求体 JSON 中提取凭证（key 或 username+password）
 * 2. 查询 users 表，LEFT JOIN 权限表聚合用户权限
 * 3. 用 libsodium Argon2id 验证凭证哈希
 * 4. 构造 JSON 响应（含用户信息 + permissions 数组）
 *
 * 权限查询关联三张表：
 * @code
 *   users ── user_permissions ── permissions
 * @endcode
 */

#include "http/login_handler.h"

#include <iostream>
#include <sstream>

#include <pqxx/pqxx>

#include "crypto/hasher.h"
#include "db/connection_pool.h"
#include "http/protocol.h"

namespace http
{

std::string handle_key_login(ConnectionPool& pool, const std::string& body)
{
    std::string key = json_extract(body, "key");
    if (key.empty())
        return build_response(400, R"({"success":false,"message":"请提供密钥"})");

    auto tx = pool.borrow();
    if (!tx)
        return build_response(500, R"({"success":false,"message":"数据库连接失败"})");

    try
    {
        pqxx::result result = tx->exec(
            "SELECT u.id, u.username, u.key_hash, "
            "  COALESCE(array_to_json(array_agg(p.permission_name) "
            "    FILTER (WHERE p.permission_name IS NOT NULL)), '[]') AS permissions "
            "FROM users u "
            "LEFT JOIN user_permissions up ON u.id = up.user_id "
            "LEFT JOIN permissions p ON up.permission_id = p.id "
            "GROUP BY u.id");

        for (auto row : result)
        {
            if (row["key_hash"].is_null())
                continue;

            std::string stored_hash = row["key_hash"].c_str();
            if (crypto::Hasher::check(key, stored_hash))
            {
                int id = row["id"].as<int>();
                std::string username = row["username"].c_str();
                std::string permissions_json = row["permissions"].c_str();
                tx->commit();

                std::ostringstream json;
                json << R"({"success":true,"message":"登录成功",)"
                     << R"("user":{"id":)" << id
                     << R"(,"username":")" << username
                     << R"(","permissions":)" << permissions_json
                     << R"(}})";
                return build_response(200, json.str());
            }
        }

        tx->commit();
        return build_response(401, R"({"success":false,"message":"不存在的密钥"})");
    }
    catch (const std::exception& e)
    {
        std::cerr << "[error] key_login: " << e.what() << '\n';
        return build_response(500, R"({"success":false,"message":"服务器内部错误"})");
    }
}

std::string handle_account_login(ConnectionPool& pool, const std::string& body)
{
    std::string username = json_extract(body, "username");
    std::string password = json_extract(body, "password");
    if (username.empty() || password.empty())
        return build_response(400, R"({"success":false,"message":"请提供用户名和密码"})");

    auto tx = pool.borrow();
    if (!tx)
        return build_response(500, R"({"success":false,"message":"数据库连接失败"})");

    try
    {
        pqxx::result result = tx->exec(
            "SELECT u.id, u.password_hash, "
            "  COALESCE(array_to_json(array_agg(p.permission_name) "
            "    FILTER (WHERE p.permission_name IS NOT NULL)), '[]') AS permissions "
            "FROM users u "
            "LEFT JOIN user_permissions up ON u.id = up.user_id "
            "LEFT JOIN permissions p ON up.permission_id = p.id "
            "WHERE u.username = $1 "
            "GROUP BY u.id",
            pqxx::params(username));

        if (result.empty())
        {
            tx->commit();
            return build_response(401, R"({"success":false,"message":"用户名不存在"})");
        }

        int id = result[0]["id"].as<int>();
        std::string stored_hash = result[0]["password_hash"].c_str();
        std::string permissions_json = result[0]["permissions"].c_str();
        tx->commit();

        if (!crypto::Hasher::check(password, stored_hash))
            return build_response(401, R"({"success":false,"message":"密码错误"})");

        std::ostringstream json;
        json << R"({"success":true,"message":"登录成功",)"
             << R"("user":{"id":)" << id
             << R"(,"username":")" << username
             << R"(","permissions":)" << permissions_json
             << R"(}})";
        return build_response(200, json.str());
    }
    catch (const std::exception& e)
    {
        std::cerr << "[error] account_login: " << e.what() << '\n';
        return build_response(500, R"({"success":false,"message":"服务器内部错误"})");
    }
}

} // namespace http