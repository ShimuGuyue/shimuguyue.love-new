/**
 * @file hasher.cpp
 * @brief Argon2id 密码哈希实现
 */

#include "crypto/hasher.h"

#include <sodium.h>

#include <array>

namespace crypto
{

/*
==================================================
   辅助 — 十六进制编解码
==================================================
*/

namespace
{

/// 原始字节（存入 string）→ 大写十六进制字符串
std::string bytes_to_hex(const std::string& data)
{
    static constexpr std::array<char, 16> hex_digits{ {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'
    } };

    std::string result(data.size() * 2, '\0');
    const auto* bytes = reinterpret_cast<const unsigned char*>(data.data());
    for (std::size_t i{ 0 }; i < data.size(); ++i)
    {
        result[i * 2]     = hex_digits[bytes[i] / 16];
        result[i * 2 + 1] = hex_digits[bytes[i] % 16];
    }
    return result;
}

/// 单个十六进制字符 → 半字节数值
unsigned char hex_char_to_nibble(char ch)
{
    if (ch >= '0' && ch <= '9')
        return static_cast<unsigned char>(ch - '0');
    if (ch >= 'a' && ch <= 'f')
        return static_cast<unsigned char>(ch - 'a' + 10);
    return static_cast<unsigned char>(ch - 'A' + 10);
}

/// 大写十六进制字符串 → 原始字节
std::string hex_to_bytes(const std::string& hex_string)
{
    std::string output(hex_string.size() / 2, '\0');
    for (std::size_t i{ 0 }; i < output.size(); ++i)
    {
        unsigned char high_nibble = hex_char_to_nibble(hex_string[i * 2]);
        unsigned char low_nibble  = hex_char_to_nibble(hex_string[i * 2 + 1]);
        output[i] = static_cast<char>(high_nibble * 16 + low_nibble);
    }
    return output;
}

} // namespace

/*
==================================================
   公开接口
==================================================
*/

std::string Hasher::get_hash(const std::string& password)
{
    if (sodium_init() < 0)
        return {};

    // 随机盐值（16 字节）
    std::array<unsigned char, salt_bytes> salt{};
    randombytes_buf(salt.data(), salt.size());

    // Argon2id 哈希（32 字节）
    std::array<unsigned char, hash_bytes> digest{};
    if (crypto_pwhash(
            digest.data(),
            digest.size(),
            password.data(),
            password.size(),
            salt.data(),
            crypto_pwhash_OPSLIMIT_MODERATE,
            crypto_pwhash_MEMLIMIT_MODERATE,
            crypto_pwhash_ALG_ARGON2ID13) != 0)
        return {};

    std::string salt_str(reinterpret_cast<const char*>(salt.data()), salt.size());
    std::string digest_str(reinterpret_cast<const char*>(digest.data()), digest.size());

    return bytes_to_hex(salt_str) + bytes_to_hex(digest_str);
}

bool Hasher::check(const std::string& password, const std::string& stored)
{
    if (sodium_init() < 0)
        return false;

    std::string decoded = hex_to_bytes(stored);

    std::array<unsigned char, hash_bytes> recomputed{};
    if (crypto_pwhash(
        recomputed.data(),
        recomputed.size(),
        password.data(),
        password.size(),
        reinterpret_cast<const unsigned char*>(decoded.data()),
        crypto_pwhash_OPSLIMIT_MODERATE,
        crypto_pwhash_MEMLIMIT_MODERATE,
        crypto_pwhash_ALG_ARGON2ID13) != 0)
        return false;

    return sodium_memcmp(
        recomputed.data(),
        reinterpret_cast<const unsigned char*>(decoded.data()) + salt_bytes,
        hash_bytes) == 0;
}

} // namespace crypto
