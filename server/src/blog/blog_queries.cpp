/**
 * @file blog/blog_queries.cpp
 * @brief 博客相关数据库查询实现
 */

#include "blog/blog_queries.h"

#include <sstream>

namespace blog {

namespace {

/// 将 int vector 转换为 PostgreSQL 数组字面量 "{1,2,3}"
auto join_ids(const std::vector<int>& ids) -> std::string
{
    if (ids.empty())
        return "{}";

    std::ostringstream oss;
    oss << "{";
    for (std::size_t i{ 0 }; i < ids.size(); ++i)
    {
        if (i > 0)
            oss << ",";
        oss << ids[i];
    }
    oss << "}";
    return oss.str();
}

} // namespace

auto get_categories(pqxx::connection& conn) -> std::vector<Category>
{
    pqxx::work txn{ conn };
    const auto rows = txn.exec("SELECT id, name FROM categories ORDER BY id");
    txn.commit();

    std::vector<Category> result;
    result.reserve(rows.size());
    for (const auto& row : rows)
    {
        result.push_back({
            row["id"].as<int>(),
            row["name"].as<std::string>()
        });
    }
    return result;
}

auto get_tags(
    pqxx::connection&    conn,
    const std::vector<int>& category_ids)
-> std::vector<Tag>
{
    pqxx::work txn{ conn };

    pqxx::result rows;
    if (category_ids.empty())
    {
        rows = txn.exec("SELECT id, name, category_id FROM tags ORDER BY id");
    }
    else
    {
        rows = txn.exec(
            "SELECT id, name, category_id FROM tags "
            "WHERE category_id = ANY($1) ORDER BY id",
            pqxx::params{ "{" + join_ids(category_ids) + "}" }
        );
    }
    txn.commit();

    std::vector<Tag> result;
    result.reserve(rows.size());
    for (const auto& row : rows)
    {
        result.push_back({
            row["id"].as<int>(),
            row["name"].as<std::string>(),
            row["category_id"].as<int>()
        });
    }
    return result;
}

auto get_blogs(
    pqxx::connection& conn,
    const BlogQuery&  query)
-> std::vector<BlogItem>
{
    pqxx::work txn{ conn };

    // 动态构建 SQL
    std::ostringstream sql;
    sql << "SELECT b.id, b.title, b.description, b.file_path, "
           "TO_CHAR(b.update_time, 'YYYY-MM-DD') AS update_time, "
           "c.name AS category_name "
           "FROM blogs b "
           "LEFT JOIN categories c ON c.id = b.category_id ";

    pqxx::params pq_params;
    bool has_where = false;
    int param_idx = 0;

    // 分类筛选（多选取并集）
    if (!query.category_ids.empty())
    {
        sql << "WHERE b.category_id = ANY($" << ++param_idx << "::int[]) ";
        pq_params.append("{" + join_ids(query.category_ids) + "}");
        has_where = true;
    }

    // 标签筛选
    if (!query.tag_ids.empty())
    {
        sql << (has_where ? "AND " : "WHERE ")
            << "b.id IN (SELECT blog_id FROM blog_tags "
            << "WHERE tag_id = ANY($" << ++param_idx << "::int[])) ";
        pq_params.append("{" + join_ids(query.tag_ids) + "}");
        has_where = true;
    }

    // 搜索（标题、描述、分类名、标签名）
    if (query.search && !query.search->empty())
    {
        sql << (has_where ? "AND " : "WHERE ")
            << "(b.title ILIKE $" << ++param_idx
            << " OR b.description ILIKE $" << param_idx
            << " OR c.name ILIKE $" << param_idx
            << " OR b.id IN ("
            << "SELECT bt.blog_id FROM blog_tags bt "
            << "JOIN tags t ON t.id = bt.tag_id "
            << "WHERE t.name ILIKE $" << param_idx
            << ")) ";
        const auto pattern = "%" + *query.search + "%";
        pq_params.append(pattern);
    }

    sql << "ORDER BY b.update_time DESC";

    const auto rows = txn.exec(sql.str(), pq_params);

    // 收集博客 ID 用于批量查询标签
    std::vector<int> blog_ids;
    blog_ids.reserve(rows.size());

    std::vector<BlogItem> result;
    result.reserve(rows.size());

    for (const auto& row : rows)
    {
        const int id = row["id"].as<int>();
        blog_ids.push_back(id);

        BlogItem item;
        item.id          = id;
        item.title       = row["title"].as<std::string>();
        item.update_time = row["update_time"].as<std::string>();

        if (!row["description"].is_null())
            item.description = row["description"].as<std::string>();

        if (!row["file_path"].is_null())
            item.file_path = row["file_path"].as<std::string>();

        if (!row["category_name"].is_null())
            item.category = row["category_name"].as<std::string>();

        result.push_back(std::move(item));
    }

    // 批量查询标签
    if (!blog_ids.empty())
    {
        const auto tag_rows = txn.exec(
            "SELECT bt.blog_id, t.name "
            "FROM blog_tags bt "
            "JOIN tags t ON t.id = bt.tag_id "
            "WHERE bt.blog_id = ANY($1) "
            "ORDER BY t.name",
            pqxx::params{ "{" + join_ids(blog_ids) + "}" }
        );

        // 按 blog_id 分组填入
        for (const auto& tr : tag_rows)
        {
            const int bid = tr["blog_id"].as<int>();
            const auto name = tr["name"].as<std::string>();
            for (auto& item : result)
            {
                if (item.id == bid)
                {
                    item.tags.push_back(name);
                    break;
                }
            }
        }
    }

    txn.commit();
    return result;
}

auto get_blog_by_file_path(
    pqxx::connection& conn,
    std::string_view  file_path)
-> std::optional<BlogItem>
{
    if (file_path.empty())
        return std::nullopt;

    pqxx::work txn{ conn };

    const auto row = txn.exec(
        "SELECT b.id, b.title, b.description, b.content, "
        "TO_CHAR(b.update_time, 'YYYY-MM-DD') AS update_time, "
        "c.name AS category_name "
        "FROM blogs b "
        "LEFT JOIN categories c ON c.id = b.category_id "
        "WHERE b.file_path = $1",
        pqxx::params{ std::string{file_path} }
    );

    if (row.empty())
        return std::nullopt;

    BlogItem item;
    item.id         = row[0]["id"].as<int>();
    item.title      = row[0]["title"].as<std::string>();
    item.update_time = row[0]["update_time"].as<std::string>();

    if (!row[0]["description"].is_null())
        item.description = row[0]["description"].as<std::string>();

    if (!row[0]["content"].is_null())
        item.content = row[0]["content"].as<std::string>();

    if (!row[0]["category_name"].is_null())
        item.category = row[0]["category_name"].as<std::string>();

    // 查询标签
    const auto tag_rows = txn.exec(
        "SELECT t.name "
        "FROM blog_tags bt "
        "JOIN tags t ON t.id = bt.tag_id "
        "WHERE bt.blog_id = $1 "
        "ORDER BY t.name",
        pqxx::params{ item.id }
    );

    for (const auto& tr : tag_rows)
    {
        item.tags.push_back(tr["name"].as<std::string>());
    }

    txn.commit();
    return item;
}

} // namespace blog
