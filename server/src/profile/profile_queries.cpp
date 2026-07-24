/**
 * @file profile/profile_queries.cpp
 * @brief 个人介绍数据库查询实现
 */

#include "profile/profile_queries.h"

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

namespace profile {

auto get_profile(pqxx::connection& conn) -> nlohmann::json
{
    pqxx::work txn{ conn };
    const auto row = txn.exec(
        "SELECT title, subtitle, bio FROM profile WHERE id = 1"
    ).one_row();
    txn.commit();

    nlohmann::json j;
    j["title"]    = row["title"].as<std::string>();
    j["subtitle"] = row["subtitle"].as<std::string>();
    j["bio"]      = row["bio"].as<std::string>();
    return j;
}

} // namespace profile
