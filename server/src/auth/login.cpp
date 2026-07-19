/**
 * @file auth/login.cpp
 * @brief 用户登录认证实现
 */

#include "auth/login.h"
#include "crypto/argon2id.h"

#include <pqxx/pqxx>

namespace auth {

namespace {

/**
 * @brief 查询用户的权限列表。
 * @param txn     当前事务。
 * @param user_id 用户 ID。
 * @return 权限列表。
 */
auto fetch_permissions(pqxx::work& txn, int user_id) -> std::vector<std::string>
{
    const auto rows = txn.exec(
        "SELECT p.name "
        "FROM user_permissions up "
        "JOIN permissions p ON p.id = up.permission_id "
        "WHERE up.user_id = $1 "
        "ORDER BY up.user_id, up.permission_id",
        pqxx::params{ user_id }
    );

    std::vector<std::string> result;
    result.reserve(rows.size());
    for (const auto& row : rows)
    {
        result.emplace_back(row["name"].as<std::string>());
    }
    return result;
}

} // namespace

auto login_by_key(
    pqxx::connection& conn, std::string_view key)
-> std::expected<LoginResult, std::string>
{
    if (key.empty())
        return std::unexpected(std::string{"密钥不能为空"});

    // 使用固定盐值对 key 做一次哈希，然后数据库精确查找
    const auto hash = crypto::Argon2id::hash_with_fixed_salt(key);
    if (!hash)
        return std::unexpected(std::string{"系统出了点问题，请稍后再试"});

    pqxx::work txn{ conn };

    // 先查该哈希是否存在（不关注是否启用）
    const auto row = txn.exec(
        "SELECT id, username, key_enabled "
        "FROM users "
        "WHERE key_hash = $1",
        pqxx::params{ *hash }
    );

    if (row.empty())
        return std::unexpected(std::string{"不存在的密钥"});
    if (!row[0]["key_enabled"].as<bool>())
        return std::unexpected(std::string{"该密钥已被废弃"});

    LoginResult result;
    result.id = row[0]["id"].as<int>();
    if (!row[0]["username"].is_null())
    {
        result.username = row[0]["username"].as<std::string>();
    }
    result.permissions = fetch_permissions(txn, result.id);
    txn.commit();
    return result;
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
    if (!crypto::Argon2id::verify(password, hash_str))
        return std::nullopt;

    LoginResult result;
    result.id = row[0]["id"].as<int>();
    result.username = std::string{username};
    result.permissions = fetch_permissions(txn, result.id);
    txn.commit();
    return result;
}

} // namespace auth
