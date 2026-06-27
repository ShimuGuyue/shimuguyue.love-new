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

namespace http
{

std::string route(ConnectionPool& pool, const Request& req)
{
    if (req.method != "POST")
        return build_response(404, R"({"success":false,"message":"仅支持 POST"})");

    if (req.path == "/api/login/key")
        return handle_key_login(pool, req.body);

    if (req.path == "/api/login/account")
        return handle_account_login(pool, req.body);

    if (req.path == "/api/blogs/import")
        return handle_blog_import(pool, req.body);

    return build_response(404, R"({"success":false,"message":"未知接口"})");
}

} // namespace http