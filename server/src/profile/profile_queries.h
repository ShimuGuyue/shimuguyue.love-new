/**
 * @file profile/profile_queries.h
 * @brief 个人介绍数据库查询
 */
#pragma once

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

#include <string>
#include <string_view>

namespace profile {

/**
 * @brief 获取个人介绍。
 * @param conn 数据库连接。
 * @return JSON 对象，含 title、subtitle、bio。
 */
[[nodiscard]] auto get_profile(pqxx::connection& conn) -> nlohmann::json;

} // namespace profile
