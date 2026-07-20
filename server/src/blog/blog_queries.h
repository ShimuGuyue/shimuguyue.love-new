/**
 * @file blog/blog_queries.h
 * @brief 博客相关数据库查询函数
 */
#pragma once

#include <optional>
#include <string>
#include <string_view>
#include <vector>

#include <nlohmann/json.hpp>
#include <pqxx/pqxx>

namespace blog {

/**
 * @brief 分类信息。
 */
struct Category
{
    int         id;
    std::string name;
};

/**
 * @brief 标签信息。
 */
struct Tag
{
    int         id;
    std::string name;
    int         category_id;
};

/**
 * @brief 博客列表项。
 */
struct BlogItem
{
    int                        id;
    std::string                title;
    std::optional<std::string> description;
    std::optional<std::string> content;
    std::optional<std::string> file_path;
    std::string                update_time;
    std::optional<std::string> category;
    std::vector<std::string>   tags;
};

/**
 * @brief 博客列表查询参数。
 */
struct BlogQuery
{
    std::vector<int>            category_ids;
    std::vector<int>            tag_ids;
    std::optional<std::string>  search;
};

/**
 * @brief 查询所有分类。
 * @param conn 数据库连接。
 * @return 分类列表。
 */
[[nodiscard]] auto get_categories(pqxx::connection& conn) -> std::vector<Category>;

/**
 * @brief 查询标签列表，可按分类筛选（多选取并集）。
 * @param conn         数据库连接。
 * @param category_ids 分类 ID 列表，空则返回全部。
 * @return 标签列表。
 */
[[nodiscard]] auto get_tags(
    pqxx::connection&       conn,
    const std::vector<int>& category_ids)
-> std::vector<Tag>;

/**
 * @brief 查询博客列表，支持筛选和搜索。
 * @param conn  数据库连接。
 * @param query 筛选参数。
 * @return 博客列表项。
 */
[[nodiscard]] auto get_blogs(
    pqxx::connection& conn,
    const BlogQuery&  query)
-> std::vector<BlogItem>;

/**
 * @brief 根据文件路径查询单篇博客。
 * @param conn      数据库连接。
 * @param file_path 博客文件相对路径。
 * @return 成功返回 BlogItem，失败返回 std::nullopt。
 */
[[nodiscard]] auto get_blog_by_file_path(
    pqxx::connection& conn,
    std::string_view  file_path)
-> std::optional<BlogItem>;

} // namespace blog
