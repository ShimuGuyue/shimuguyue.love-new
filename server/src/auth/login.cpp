/**
 * @file auth/login.cpp
 * @brief 用户登录认证实现
 */

#include "auth/login.h"
#include "crypto/argon2id.h"

#include <pqxx/pqxx>

namespace auth {

auto login_by_key(
    pqxx::connection& conn, std::string_view key)
-> std::optional<LoginResult>
{
    if (key.empty())
        return std::nullopt;

    pqxx::work txn{ conn };

    // 查询所有启用的 key
    const auto rows = txn.exec(
        "SELECT id, key_hash, username "
        "FROM users "
        "WHERE key_enabled = TRUE"
    );

    for (const auto& row : rows)
    {
        const auto hash = row["key_hash"].as<std::string>();
        if (crypto::Argon2id::verify(hash, key))
        {
            LoginResult result;
            result.id = row["id"].as<int>();
            if (!row["username"].is_null())
            {
                result.username = row["username"].as<std::string>();
            }
            txn.commit();
            return result;
        }
    }

    return std::nullopt;
}

auto login_by_password(
    pqxx::connection& conn,
    std::string_view  username,
    std::string_view  password)
-> std::optional<LoginResult>
{
    if (username.empty() || password.empty())
        return std::nullopt;

    pqxx::work txn{ conn };

    const auto row = txn.exec(
        "SELECT id, password_hash "
        "FROM users "
        "WHERE username = $1",
        pqxx::params{ std::string{username} }
    );

    if (row.empty())
        return std::nullopt;

    const auto hash = row[0]["password_hash"];

    // 用户可能只通过 key 认证，未设置密码
    if (hash.is_null())
        return std::nullopt;

    const auto hash_str = hash.as<std::string>();
    if (!crypto::Argon2id::verify(hash_str, password))
        return std::nullopt;

    LoginResult result;
    result.id = row[0]["id"].as<int>();
    result.username = std::string{username};
    txn.commit();
    return result;
}

} // namespace auth
