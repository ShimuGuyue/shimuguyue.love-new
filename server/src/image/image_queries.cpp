/**
 * @file image/image_queries.cpp
 * @brief 照片墙图片数据库查询实现
 */

#include "image/image_queries.h"

#include <cstdlib>
#include <filesystem>
#include <format>
#include <iostream>
#include <nlohmann/json.hpp>

namespace img {

static std::string IMAGE_PATH;

void init()
{
    std::cout << "正在获取 IMAGE_PATH..." << std::endl;
    const char* p = std::getenv("IMAGE_PATH");
    if (!p)
    {
        std::cerr << "错误：缺少必需的环境变量 IMAGE_PATH！" << std::endl;
        std::exit(1);
    }
    IMAGE_PATH = p;

    // 确保 home 子目录存在
    std::error_code ec;
    std::filesystem::create_directories(
        std::format("{}/home", IMAGE_PATH), ec);

    std::cout << "IMAGE_PATH 获取成功。\n" << std::endl;
}

auto image_path() -> const std::string&
{
    return IMAGE_PATH;
}

auto get_all_images(pqxx::connection& conn) -> nlohmann::json
{
    pqxx::work txn{ conn };
    const auto rows = txn.exec(
        "SELECT id, path, description, scale, rotation, pos_x, pos_y "
        "FROM images "
        "ORDER BY id"
    );
    txn.commit();

    nlohmann::json arr = nlohmann::json::array();
    for (const auto& row : rows)
    {
        nlohmann::json item;
        item["id"]          = row["id"].as<int>();
        item["path"]        = row["path"].as<std::string>();
        item["description"] = row["description"].as<std::string>();
        item["scale"]       = row["scale"].as<double>();
        item["rotation"]    = row["rotation"].as<double>();
        item["pos_x"]       = row["pos_x"].as<double>();
        item["pos_y"]       = row["pos_y"].as<double>();
        arr.push_back(std::move(item));
    }
    return arr;
}

auto save_image(
    pqxx::connection&  conn,
    std::string_view   path,
    std::string_view   description,
    double             scale,
    double             rotation,
    double             pos_x,
    double             pos_y)
-> std::string
{
    pqxx::work txn{ conn };

    // UPSERT: 存在则更新元数据，不存在则插入
    txn.exec(
        "INSERT INTO images (path, description, scale, rotation, pos_x, pos_y) "
        "VALUES ($1, $2, $3, $4, $5, $6) "
        "ON CONFLICT (path) DO UPDATE SET "
        "description = EXCLUDED.description, "
        "scale       = EXCLUDED.scale, "
        "rotation    = EXCLUDED.rotation, "
        "pos_x       = EXCLUDED.pos_x, "
        "pos_y       = EXCLUDED.pos_y",
        pqxx::params{
            std::string{path},
            std::string{description},
            scale,
            rotation,
            pos_x,
            pos_y
        }
    );

    txn.commit();
    return {};
}
} // namespace img
