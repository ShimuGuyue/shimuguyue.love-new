/**
 * @file config.h
 * @brief 服务配置
 *
 * @details 全部配置集中于此。
 * - 数据库连接：~/.pg_service.conf + ~/.pgpass
 * - 博客存储路径：环境变量 BLOG_ROOT，启动时强制校验
 */

#pragma once
#include <string>
#include <cstdlib>
#include <optional>

namespace config
{

inline std::string db_connection_string() { return "service=love"; }

inline int pool_size() { return 5; }

/**
 * @brief 博客文档存储根目录（绝对路径）
 *
 * @return 环境变量 BLOG_ROOT 的值（末尾保证有 /）；
 *         未设置返回 std::nullopt
 */
inline std::optional<std::string> blog_root()
{
    const char* env = std::getenv("BLOG_ROOT");
    if (!env || env[0] == '\0')
        return std::nullopt;

    std::string path = env;
    if (path.back() != '/')
        path += '/';
    return path;
}

} // namespace config