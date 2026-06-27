/**
 * @file blog_query.cpp
 * @brief 博客查询接口实现
 *
 * @details
 * 只读操作：从未关闭的连接池借用事务，查询数据库或读取文件系统。
 *
 * 辅助函数（匿名命名空间）：
 * - url_decode()      将 %xx 编码还原为原始字符
 * - escape_json_str() 对字符串做最小 JSON 转义（" → \\"、\ → \\\\、换行 → \\n）
 * - read_file()       读取文件全部内容
 */

#include "http/blog_query.h"

#include <algorithm>
#include <fstream>
#include <sstream>
#include <iostream>
#include <cctype>
#include <vector>

#include <pqxx/pqxx>

#include "config/config.h"
#include "db/connection_pool.h"
#include "http/protocol.h"

namespace http
{

namespace
{

/**
 * @brief URL 解码 — 将 %xx 还原为原始字符
 *
 * @details 支持 %20（空格）、%E4%B8%AD（中文 UTF-8）等。
 *          % 后不是两个 hex 字符则原样保留。
 */
std::string url_decode(const std::string& src)
{
    std::string result;
    result.reserve(src.size());
    for (std::size_t i = 0; i < src.size(); ++i)
    {
        if (src[i] == '%' && i + 2 < src.size()
            && std::isxdigit(static_cast<unsigned char>(src[i + 1]))
            && std::isxdigit(static_cast<unsigned char>(src[i + 2])))
        {
            int value = std::stoi(src.substr(i + 1, 2), nullptr, 16);
            result.push_back(static_cast<char>(value));
            i += 2;
        }
        else if (src[i] == '+')
        {
            result.push_back(' ');
        }
        else
        {
            result.push_back(src[i]);
        }
    }
    return result;
}

/**
 * @brief JSON 字符串值转义
 *
 * @details 对 " \ 换行 进行转义；不处理 Unicode。
 */
std::string escape_json_str(const std::string& s)
{
    std::string result;
    result.reserve(s.size() * 2);
    for (char ch : s)
    {
        switch (ch)
        {
        case '"':  result += "\\\""; break;
        case '\\': result += "\\\\"; break;
        case '\n': result += "\\n";  break;
        case '\r': result += "\\r";  break;
        case '\t': result += "\\t";  break;
        default:   result.push_back(ch);
        }
    }
    return result;
}

/**
 * @brief 清理字符串为安全文件名（与 blog_import.cpp 中的 safe_name 一致）
 *
 * @details 去除路径分隔符和不可打印字符。
 */
std::string safe_name(std::string s)
{
    s.erase(std::remove_if(s.begin(), s.end(), [](char c)
    {
        return c == '/' || c == '\\' || static_cast<unsigned char>(c) < 0x20;
    }), s.end());
    if (s.empty()) s = "untitled";
    return s;
}

/**
 * @brief 读取文件全部文本内容
 *
 * @param path 文件绝对路径
 * @return 文件内容；打开失败返回空字符串
 */
std::string read_file(const std::string& path)
{
    std::ifstream in(path);
    if (!in)
        return {};
    std::ostringstream oss;
    oss << in.rdbuf();
    return oss.str();
}

} // namespace

std::string handle_blog_list(ConnectionPool& pool)
{
    auto tx = pool.borrow();
    if (!tx)
        return build_response(500,
            R"({"success":false,"message":"数据库连接失败"})");

    try
    {
        pqxx::result blog_rows = tx->exec(
            "SELECT b.id, b.title, b.description, b.time, c.name AS category "
            "FROM blogs b "
            "JOIN blog_categories c ON b.category_id = c.id "
            "ORDER BY b.id DESC");

        std::ostringstream json;
        json << R"({"success":true,"blogs":[)";

        bool first_blog = true;
        for (auto const& row : blog_rows)
        {
            if (!first_blog) json << ',';
            first_blog = false;

            int blog_id = row["id"].as<int>();
            std::string title       = row["title"].c_str();
            std::string category    = row["category"].c_str();
            std::string description = row["description"].is_null() ? "" : row["description"].c_str();
            std::string time_val    = row["time"].is_null() ? "" : row["time"].c_str();
            if (time_val.size() > 10) time_val.resize(10);

            // 查询该文章的标签
            pqxx::result tag_rows = tx->exec(
                "SELECT t.name FROM blog_tags t "
                "JOIN post_tags pt ON t.id = pt.tag_id "
                "WHERE pt.post_id = $1 "
                "ORDER BY t.name",
                pqxx::params(blog_id));

            json << "{\"id\":" << blog_id
                 << ",\"title\":\"" << escape_json_str(title) << '"'
                 << ",\"category\":\"" << escape_json_str(category) << '"'
                 << ",\"description\":\"" << escape_json_str(description) << '"'
                 << ",\"time\":\"" << escape_json_str(time_val) << '"'
                 << ",\"tags\":[";

            bool first_tag = true;
            for (auto const& tr : tag_rows)
            {
                if (!first_tag) json << ',';
                first_tag = false;
                json << '"' << escape_json_str(tr["name"].c_str()) << '"';
            }

            json << "]}";
        }

        json << "]}";
        tx->commit();
        return build_response(200, json.str());
    }
    catch (const std::exception& e)
    {
        std::cerr << "[error] blog_list: " << e.what() << '\n';
        return build_response(500,
            R"({"success":false,"message":"查询失败"})");
    }
}

std::string handle_blog_article(ConnectionPool& pool, const std::string& raw_title)
{
    std::string title = url_decode(raw_title);
    if (title.empty())
        return build_response(400,
            R"({"success":false,"message":"标题不能为空"})");

    auto tx = pool.borrow();
    if (!tx)
        return build_response(500,
            R"({"success":false,"message":"数据库连接失败"})");

    try
    {
        // 查询博客元数据
        pqxx::result blog_rows = tx->exec(
            "SELECT b.id, b.title, c.name AS category "
            "FROM blogs b "
            "JOIN blog_categories c ON b.category_id = c.id "
            "WHERE b.title = $1 "
            "ORDER BY b.id DESC "
            "LIMIT 1",
            pqxx::params(title));

        if (blog_rows.empty())
            return build_response(404,
                R"({"success":false,"message":"文章不存在"})");

        int blog_id          = blog_rows[0]["id"].as<int>();
        std::string category = blog_rows[0]["category"].c_str();

        // 查询标签
        pqxx::result tag_rows = tx->exec(
            "SELECT t.name FROM blog_tags t "
            "JOIN post_tags pt ON t.id = pt.tag_id "
            "WHERE pt.post_id = $1 "
            "ORDER BY t.name",
            pqxx::params(blog_id));

        tx->commit();

        // 读取 Markdown 文件
        auto root_opt = config::blog_root();
        if (!root_opt)
            return build_response(500,
                R"({"success":false,"message":"服务器未配置 BLOG_ROOT"})");

        std::string file_path = *root_opt + safe_name(category) + "/" + safe_name(title) + ".md";
        std::string content = read_file(file_path);

        if (content.empty())
        {
            // 文件可能不存在，但仍返回元数据
            std::cerr << "[warn] blog_article: 文件不存在 " << file_path << '\n';
        }

        // 构建 JSON 响应
        std::ostringstream json;
        json << "{\"success\":true"
             << ",\"id\":" << blog_id
             << ",\"title\":\"" << escape_json_str(title) << '"'
             << ",\"category\":\"" << escape_json_str(category) << '"'
             << ",\"tags\":[";

        bool first_tag = true;
        for (auto const& tr : tag_rows)
        {
            if (!first_tag) json << ',';
            first_tag = false;
            json << '"' << escape_json_str(tr["name"].c_str()) << '"';
        }

        json << "]"
             << ",\"content\":\"" << escape_json_str(content) << '"'
             << "}";

        return build_response(200, json.str());
    }
    catch (const std::exception& e)
    {
        std::cerr << "[error] blog_article: " << e.what() << '\n';
        return build_response(500,
            R"({"success":false,"message":"查询失败"})");
    }
}

} // namespace http
