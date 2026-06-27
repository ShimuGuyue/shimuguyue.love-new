/**
 * @file handlers.cpp
 * @brief HTTP 路由分发实现
 *
 * @details 单一职责：仅做路由分发，具体业务逻辑委托给各 handler 模块。
 */

#include "http/handlers.h"

#include "db/connection_pool.h"
#include "http/protocol.h"
#include "http/login_handler.h"
#include "http/blog_import.h"
#include "http/blog_query.h"

namespace http
{

std::string route(ConnectionPool& pool, const Request& req)
{
    // --- GET 路由 ---
    if (req.method == "GET")
    {
        if (req.path == "/api/blogs")
            return handle_blog_list(pool);

        // /api/blog/{title}
        if (req.path.rfind("/api/blog/", 0) == 0)
        {
            std::string raw_title = req.path.substr(std::string("/api/blog/").size());
            if (!raw_title.empty())
                return handle_blog_article(pool, raw_title);
        }
    }

    // --- POST 路由 ---
    if (req.method == "POST")
    {
        if (req.path == "/api/login/key")
            return handle_key_login(pool, req.body);

        if (req.path == "/api/login/account")
            return handle_account_login(pool, req.body);

        if (req.path == "/api/blogs/import")
            return handle_blog_import(pool, req.body);
    }

    return build_response(404, R"({"success":false,"message":"未知接口"})");
}

} // namespace http