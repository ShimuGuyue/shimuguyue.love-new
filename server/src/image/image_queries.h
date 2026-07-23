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

/**
 * @brief 保存图片记录（不存在则插入，存在则更新位置/缩放/旋转/简介）。
 * @param conn        数据库连接。
 * @param path        图片相对路径。
 * @param description 简介。
 * @param scale       缩放比例。
 * @param rotation    旋转角度。
 * @param pos_x       水平坐标（百分比）。
 * @param pos_y       垂直坐标（百分比）。
 * @return 错误消息（空表示成功）。
 */
[[nodiscard]] auto save_image(
    pqxx::connection&  conn,
    std::string_view   path,
    std::string_view   description,
    double             scale,
    double             rotation,
    double             pos_x,
    double             pos_y)
-> std::string;

} // namespace img
