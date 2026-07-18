/**
 * @file crypto/argon2id.cpp
 * @brief Argon2id 密码哈希实现
 */

#include "crypto/argon2id.h"

#include <sodium.h>

namespace crypto {

auto Argon2id::hash(std::string_view password) -> std::optional<std::string>
{
    if (password.empty())
    {
        return std::nullopt;
    }

    std::string res(crypto_pwhash_STRBYTES, '\0');
    const auto result = static_cast<int>(
        crypto_pwhash_str(
            res.data(),
            password.data(),
            password.size(),
            crypto_pwhash_OPSLIMIT_SENSITIVE,
            crypto_pwhash_MEMLIMIT_SENSITIVE
        )
    );

    if (result != 0)
        return std::nullopt;

    // 去除 crypto_pwhash_STRBYTES 尾部包含的 '\0'，返回紧凑字符串
    res.resize(std::char_traits<char>::length(res.data()));
    return res;
}

auto Argon2id::verify(std::string_view hash, std::string_view password) -> bool
{
    if (hash.empty() || password.empty())
        return false;

    auto result = static_cast<int>(
        crypto_pwhash_str_verify(
            hash.data(),
            password.data(),
            password.size()
        )
    );

    return result == 0;
}

} // namespace crypto
