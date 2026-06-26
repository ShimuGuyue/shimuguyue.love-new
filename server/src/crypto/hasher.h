/**
 * @file hasher.h
 * @brief Argon2id 密码哈希工具
 *
 * @details
 * 使用 libsodium 的 crypto_pwhash 接口实现 Argon2id 密码哈希。
 * - 哈希输出 32 字节（64 十六进制字符）
 * - 随机盐值 16 字节 + 哈希 32 字节 = 96 十六进制字符存储字符串
 * - 由调用方保证输入合法，本类不做校验
 */

#pragma once

#include <string>

namespace crypto
{

class Hasher
{
private:
    static constexpr int hash_bytes{ 32 };
    static constexpr int salt_bytes{ 16 };

public:
    Hasher()  = delete;
    ~Hasher() = delete;

public:
    /// 对密码执行 Argon2id 哈希，返回 96 字符存储字符串
    static std::string get_hash(const std::string& password);

    /// 验证明文密码是否与存储哈希匹配
    static bool check(const std::string& password, const std::string& stored);
};

} // namespace crypto
