/**
 * @file blog_import.h
 * @brief 博客导入接口
 *
 * @details
 * 接收前端 BlogImportPage 提交的 JSON，完成：
 * - Markdown 正文写入磁盘（含 YAML frontmatter 头信息）
 * - 类别、标签写入数据库（blog_categories / blog_tags / post_tags）
 * - 文章记录写入 blogs 表
 */

#pragma once

#include <string>

class ConnectionPool;

namespace http
{

/**
 * @brief 处理博客导入
 *
 * @details
 * 请求体 JSON 格式：
 * @code
 * {
 *   "title":    "文章标题",
 *   "category": "技术",
 *   "tags":     "Vue, TypeScript",
 *   "body":     "# 标题\n\n正文..."
 * }
 * @endcode
 *
 * 处理流程：
 * 1. 解析 JSON 提取各字段，body 做 JSON 反转义（\\n → 换行）
 * 2. 以 BLOG_ROOT/category/title.md 写入文件系统
 * 3. UPSERT 类别到 blog_categories
 * 4. INSERT 文章到 blogs
 * 5. UPSERT 标签到 blog_tags + 关联到 post_tags
 *
 * @param pool 数据库连接池
 * @param body 请求体 JSON
 * @return JSON 响应 @c {"success":true,"id":...} 或 @c {"success":false,"message":"..."}
 */
std::string handle_blog_import(ConnectionPool& pool, const std::string& body);

} // namespace http