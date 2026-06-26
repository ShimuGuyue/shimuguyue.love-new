/**
 * @file handlers.cpp
 * @brief 登录处理 + 路由实现
 */

#include "http/handlers.h"

#include <iostream>
#include <sstream>

#include <pqxx/pqxx>

#include "crypto/hasher.h"
#include "db/connection_pool.h"

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

std::string route(ConnectionPool& pool, const Request& req)
{
    if (req.method != "POST")
        return build_response(404, R"({"success":false,"message":"仅支持 POST"})");

    if (req.path == "/api/login/key")
        return handle_key_login(pool, req.body);

    if (req.path == "/api/login/account")
        return handle_account_login(pool, req.body);

    return build_response(404, R"({"success":false,"message":"未知接口"})");
}

} // namespace http