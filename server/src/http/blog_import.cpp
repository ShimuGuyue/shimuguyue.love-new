/**
 * @file blog_import.cpp
 * @brief 博客导入实现
 *
 * @details
 * 导入一个 Markdown 文档的完整流程：
 *
 * @dot
 *   digraph import_flow {
 *     rankdir=TB;
 *     node [shape=box];
 *     parse [label="解析 JSON\n提取 title/category/tags/body"];
 *     sanitize [label="安全清洗\nsafe_name() 去除 / 和控制字符"];
 *     mkdir [label="创建目录\nmkdir_p() 递归创建"];
 *     write [label="写文件\n含 YAML frontmatter 头"];
 *     db [label="写数据库\nblog_categories → blogs → blog_tags → post_tags"];
 *     parse -> sanitize -> mkdir -> write -> db;
 *   }
 * @enddot
 *
 * 辅助函数（匿名命名空间）：
 * - trim()        去除首尾空白
 * - safe_name()   文件名安全清洗
 * - mkdir_p()     递归创建目录
 * - unescape()    JSON 转义反转义
 * - extract_body()从原始 JSON 中提取 body 字段值
 */

#include "http/blog_import.h"

#include <fstream>
#include <iostream>
#include <sstream>
#include <sys/stat.h>
#include <cerrno>
#include <cstring>
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
 * @brief 去除字符串首尾空白（空格、制表、回车、换行）
 */
std::string trim(std::string s)
{
    auto start = s.find_first_not_of(" \t\r\n");
    if (start == std::string::npos) return "";
    auto end = s.find_last_not_of(" \t\r\n");
    return s.substr(start, end - start + 1);
}

/**
 * @brief 清理字符串为安全文件名
 *
 * @details 去除路径分隔符（/ \\）和不可打印字符（ASCII < 0x20）。
 *          保留 UTF-8 多字节字符（每字节 ≥ 0x80）。\n
 *          空字符串回退为 "untitled"。
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
 * @brief 递归创建目录（类似 mkdir -p）
 *
 * @param path 目标路径（绝对路径）
 * @param err  失败时写入错误描述
 * @return true 成功或已存在；false 失败
 */
bool mkdir_p(const std::string& path, std::string& err)
{
    std::string cur;
    for (std::size_t i = 0; i < path.size(); ++i)
    {
        cur.push_back(path[i]);
        if (path[i] == '/' || i == path.size() - 1)
        {
            if (!cur.empty() && cur != "/")
            {
                struct stat st;
                if (stat(cur.c_str(), &st) != 0)
                {
                    if (mkdir(cur.c_str(), 0755) != 0)
                    {
                        err = "mkdir(" + cur + "): " + strerror(errno);
                        return false;
                    }
                }
                else if (!S_ISDIR(st.st_mode))
                {
                    err = cur + " 已存在但不是目录";
                    return false;
                }
            }
        }
    }
    return true;
}

/**
 * @brief JSON 转义反转义
 *
 * @details 将 JSON 字符串中的转义序列还原为实际字符：
 *          \\n → 换行，\\t → 制表，\\r → 回车，\\" → 双引号，\\\\ → 反斜杠
 */
std::string unescape(const std::string& s)
{
    std::string result;
    result.reserve(s.size());
    for (std::size_t i = 0; i < s.size(); ++i)
    {
        if (s[i] == '\\' && i + 1 < s.size())
        {
            switch (s[i + 1])
            {
            case 'n':  result.push_back('\n'); ++i; break;
            case 't':  result.push_back('\t'); ++i; break;
            case 'r':  result.push_back('\r'); ++i; break;
            case '"':  result.push_back('"');  ++i; break;
            case '\\': result.push_back('\\'); ++i; break;
            default:   result.push_back(s[i]); break;
            }
        }
        else
        {
            result.push_back(s[i]);
        }
    }
    return result;
}

/**
 * @brief 从原始 JSON 中提取 body 字段值
 *
 * @details 搜索 @c "body":" 标记，跳过 8 字符（含值的开引号），
 *          以 @c "} 定位结尾，返回中间内容（不含转义处理）。
 *
 * @param json 原始请求体 JSON
 * @return body 字段的原始字符串值
 */
std::string extract_body(const std::string& json)
{
    auto pos = json.find(R"("body":")");
    if (pos == std::string::npos) return {};

    pos += 8;
    auto end = json.rfind("\"}");
    if (end == std::string::npos || end < pos) return {};

    return json.substr(pos, end - pos);
}

} // namespace

std::string handle_blog_import(ConnectionPool& pool, const std::string& body)
{
    std::string title       = json_extract(body, "title");
    std::string category    = trim(json_extract(body, "category"));
    std::string tags_str    = json_extract(body, "tags");
    std::string description = json_extract(body, "description");
    std::string content     = unescape(extract_body(body));

    if (title.empty() || content.empty())
        return build_response(400,
            R"({"success":false,"message":"标题和正文不能为空"})");
    if (category.empty())
        return build_response(400,
            R"({"success":false,"message":"类别不能为空"})");

    auto root_opt = config::blog_root();
    if (!root_opt)
        return build_response(500,
            R"({"success":false,"message":"服务器未配置 BLOG_ROOT"})");

    std::string file_dir  = *root_opt + safe_name(category);
    std::string file_path = file_dir + "/" + safe_name(title) + ".md";

    std::string err;
    if (!mkdir_p(file_dir, err))
    {
        std::cerr << "[error] 创建目录失败: " << file_dir << " — " << err << '\n';
        return build_response(500,
            R"({"success":false,"message":"无法创建目录"})");
    }

    // 解析标签
    std::vector<std::string> tag_list;
    {
        std::istringstream ss(tags_str);
        std::string t;
        while (std::getline(ss, t, ','))
        {
            t = trim(t);
            if (!t.empty()) tag_list.push_back(t);
        }
    }

    // 写文件（含 YAML frontmatter）
    {
        std::ofstream out(file_path);
        if (!out)
            return build_response(500,
                R"({"success":false,"message":"无法写入文件"})");

        out << "---\n"
            << "categorie: " << category << "\n"
            << "description: " << description << "\n"
            << "tags: [";
        for (std::size_t i = 0; i < tag_list.size(); ++i)
        {
            if (i > 0) out << ", ";
            out << tag_list[i];
        }
        out << "]\n"
            << "---\n\n"
            << content;
    }

    auto tx = pool.borrow();
    if (!tx)
    {
        std::remove(file_path.c_str());
        return build_response(500, R"({"success":false,"message":"数据库连接失败"})");
    }

    try
    {
        // UPSERT 类别
        pqxx::result cat_res = tx->exec(
            "INSERT INTO blog_categories (name) VALUES ($1) "
            "ON CONFLICT (name) DO UPDATE SET name = EXCLUDED.name "
            "RETURNING id",
            pqxx::params(category));
        int category_id = cat_res[0]["id"].as<int>();

        // INSERT 文章
        pqxx::result blog_res = tx->exec(
            "INSERT INTO blogs (title, category_id, description) VALUES ($1, $2, $3) "
            "RETURNING id",
            pqxx::params(title, category_id, description));
        int blog_id = blog_res[0]["id"].as<int>();

        // UPSERT 标签 + 关联
        for (const auto& tag : tag_list)
        {
            auto tag_res = tx->exec(
                "INSERT INTO blog_tags (name, category_id) VALUES ($1, $2) "
                "ON CONFLICT (category_id, name) DO UPDATE SET name = EXCLUDED.name "
                "RETURNING id",
                pqxx::params(tag, category_id));
            int tag_id = tag_res[0]["id"].as<int>();

            tx->exec(
                "INSERT INTO post_tags (post_id, tag_id) VALUES ($1, $2) "
                "ON CONFLICT DO NOTHING",
                pqxx::params(blog_id, tag_id));
        }

        tx->commit();

        std::ostringstream json;
        json << R"({"success":true,"message":"导入成功","id":)" << blog_id << "}";
        return build_response(200, json.str());
    }
    catch (const std::exception& e)
    {
        std::cerr << "[error] blog_import: " << e.what() << '\n';
        std::remove(file_path.c_str());
        return build_response(500, R"({"success":false,"message":"数据库写入失败"})");
    }
}

} // namespace http