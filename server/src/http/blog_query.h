/**
 * @file blog_query.h
 * @brief 博客查询接口 — 列表 / 详情
 *
 * @details
 * 提供两个只读 API：
 * - GET /api/blogs        返回所有博客的类别 + 标签
 * - GET /api/blog/{title}  返回单篇文章的 Markdown 全文
 */

#pragma once

#include <string>

class ConnectionPool;

namespace http
{

/**
 * @brief 获取博客列表（含类别和标签）
 *
 * @details
 * 查询 blogs 表，JOIN blog_categories 得到分类名，
 * 再通过 post_tags + blog_tags 收集每篇文章的标签。
 *
 * 响应 JSON：
 * @code
 * {
 *   "success": true,
 *   "blogs": [
 *     {
 *       "id": 1,
 *       "title": "文章标题",
 *       "category": "技术",
 *       "tags": ["Vue", "TypeScript"]
 *     }
 *   ]
 * }
 * @endcode
 *
 * @param pool 数据库连接池
 * @return HTTP JSON 响应
 */
std::string handle_blog_list(ConnectionPool& pool);

/**
 * @brief 获取单篇博客详情（Markdown 全文）
 *
 * @details
 * 1. 通过 title 查询 blogs 表获取 category
 * 2. 读取 BLOG_ROOT/{category}/{title}.md
 * 3. 返回 JSON（含 Markdown 原文及元数据）
 *
 * URL 中的 title 会被 URL 解码（%xx → 原始字符）。
 *
 * 响应 JSON：
 * @code
 * {
 *   "success": true,
 *   "id": 1,
 *   "title": "文章标题",
 *   "category": "技术",
 *   "tags": ["Vue", "TypeScript"],
 *   "content": "# 标题\n\n正文..."
 * }
 * @endcode
 *
 * @param pool       数据库连接池
 * @param raw_title  URL 路径中的标题（可能含 %xx 编码）
 * @return HTTP JSON 响应
 */
std::string handle_blog_article(ConnectionPool& pool, const std::string& raw_title);

} // namespace http
