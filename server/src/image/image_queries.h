/**
 * @file image/image_queries.h
 * @brief 照片墙图片数据库查询
 */
#pragma once

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

#include <string>
#include <string_view>
#include <vector>

namespace img {

/**
 * @brief 初始化：读取 IMAGE_PATH 环境变量，不存在则 exit(1)。
 */
void init();

/**
 * @brief 获取 IMAGE_PATH。
 */
[[nodiscard]] auto image_path() -> const std::string&;

/**
 * @brief 获取所有图片记录。
 * @param conn 数据库连接。
 * @return JSON 数组。
 */
[[nodiscard]] auto get_all_images(pqxx::connection& conn) -> nlohmann::json;
