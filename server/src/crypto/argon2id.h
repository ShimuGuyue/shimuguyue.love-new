/**
 * @file crypto/argon2id.h
 * @brief Argon2id 密码哈希类 — 基于 libsodium 高级 API
 */
#pragma once

#include <optional>
#include <string>
#include <string_view>

namespace crypto {

/**
 * @brief Argon2id 密码哈希工具类。
 *
 * 使用 libsodium 的 crypto_pwhash_str / crypto_pwhash_str_verify 高级 API，
 * 自动生成随机盐，返回 $argon2id$... 格式的编码哈希字符串。
 * 安全参数使用 SENSITIVE 级别（~1 GB 内存，~2 秒耗时）。
 */
class Argon2id
{
public:
    Argon2id() = delete;

    /**
     * @brief 对明文密码进行 Argon2id 哈希。
     * @param password 明文密码。
     * @return 成功返回编码后的哈希字符串（含算法标识、参数、盐和哈希值），
     *         失败返回 std::nullopt。
     */
    [[nodiscard]] static auto hash(std::string_view password) -> std::optional<std::string>;

    /**
     * @brief 验证明文密码是否与哈希值匹配。
     * @param hash 明文密码对应的哈希值。
     * @param password 待验证的明文密码。
     * @return 匹配明文密码与哈希值匹配结果。
     */
    [[nodiscard]] static auto verify(std::string_view hash, std::string_view password) -> bool;
};

} // namespace crypto
