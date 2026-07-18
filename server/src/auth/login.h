/**
 * @file auth/login.h
 * @brief 用户登录认证函数定义
 */
#pragma once

#include <optional>
#include <string>
#include <string_view>

#include <pqxx/pqxx>

namespace auth {

/**
 * @brief 登录成功后的返回结果。
 */
struct LoginResult
{
    int                        id;
    std::optional<std::string> username;
};

/**
 * @brief 通过 Key 登录。
 *
 * 遍历所有 key_enabled = TRUE 的用户，逐行用 Argon2id 验证 key。
 *
 * @param conn 数据库连接。
 * @param key  用户输入的原始 key。
 * @return 成功返回 LoginResult，失败返回 std::nullopt。
 */
[[nodiscard]] auto login_by_key(
    pqxx::connection& conn, std::string_view key)
-> std::optional<LoginResult>;

/**
 * @brief 通过用户名密码登录。
 *
 * 通过 username 精确定位用户行，用 Argon2id 验证密码。
 *
 * @param conn     数据库连接。
 * @param username 用户名。
 * @param password 明文密码。
 * @return 成功返回 LoginResult，失败返回 std::nullopt。
 */
[[nodiscard]] auto login_by_password(
    pqxx::connection& conn,
    std::string_view  username,
    std::string_view  password)
-> std::optional<LoginResult>;

} // namespace auth
