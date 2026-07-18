/**
 * @file http/routes.cpp
 * @brief HTTP 路由实现
 */

#include "http/routes.h"
#include "auth/login.h"

#include <cstdlib>
#include <iostream>
#include <nlohmann/json.hpp>

namespace http {

// ── 环境变量读取 ──

auto read_port_or_exit() -> int
{
    /*log*/std::cout << "正在获取 SERVER_PORT..." << std::endl;
    const char* port_str = std::getenv("SERVER_PORT");
    if (port_str == nullptr)
    {
        /*log*/std::cerr << "错误：缺少必需的环境变量 SERVER_PORT！" << std::endl;
        std::exit(1);
    }
    const int port = std::atoi(port_str);
    if (port <= 0)
    {
        /*log*/std::cerr << "错误：SERVER_PORT 必须是有效的端口号！" << std::endl;
        std::exit(1);
    }
    /*log*/std::cout << "SERVER_PORT 获取成功。\n" << std::endl;
    return port;
}

auto read_frontend_origin_or_exit() -> std::string
{
    /*log*/std::cout << "正在获取 FRONTEND_ORIGIN..." << std::endl;
    const char* origin = std::getenv("FRONTEND_ORIGIN");
    if (origin == nullptr)
    {
        /*log*/std::cerr << "错误：缺少必需的环境变量 FRONTEND_ORIGIN！" << std::endl;
        std::exit(1);
    }
    /*log*/std::cout << "FRONTEND_ORIGIN 获取成功。\n" << std::endl;
    return origin;
}

auto read_host_or_exit() -> std::string
{
    /*log*/std::cout << "正在获取 SERVER_HOST..." << std::endl;
    const char* host = std::getenv("SERVER_HOST");
    if (host == nullptr)
    {
        /*log*/std::cerr << "错误：缺少必需的环境变量 SERVER_HOST！" << std::endl;
        std::exit(1);
    }
    /*log*/std::cout << "SERVER_HOST 获取成功。\n" << std::endl;
    return host;
}

// ── 路由处理函数 ──

/**
 * @brief CORS 预检响应。
 */
static void handle_cors(httplib::Response& res, const std::string& allowed)
{
    res.set_header("Access-Control-Allow-Origin",  allowed);
    res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
    res.set_header("Access-Control-Allow-Headers", "Content-Type");
    res.status = 204;
}

/**
 * @brief 处理 POST /api/login/key 请求。
 */
static void handle_login_key(
    const httplib::Request&  req,
    httplib::Response&       res,
    pqxx::connection&        conn,
    const std::string&       allowed)
{
    res.set_header("Access-Control-Allow-Origin", allowed);
    res.set_header("Content-Type", "application/json");

    // 解析 JSON
    const auto body = nlohmann::json::parse(req.body, nullptr, false);
    if (body.is_discarded())
    {
        res.status = 400;
        res.set_content(R"({"error":"无效的 JSON"})", "application/json");
        return;
    }

    // 调用登录逻辑
    const auto key = body.value("key", "");
    auto result = auth::login_by_key(conn, key);

    // 返回结果
    if (!result)
    {
        res.status = 401;
        res.set_content(R"({"error":"不存在的密钥"})", "application/json");
        return;
    }
    nlohmann::json resp;
    resp["id"] = result->id;
    resp["username"] = result->username.has_value()
        ? nlohmann::json(*result->username)
        : nlohmann::json(nullptr);
    res.set_content(resp.dump(), "application/json");
}

/**
 * @brief 处理 POST /api/login/password 请求。
 */
static void handle_login_password(
    const httplib::Request&  req,
    httplib::Response&       res,
    pqxx::connection&        conn,
    const std::string&       allowed)
{
    res.set_header("Access-Control-Allow-Origin", allowed);
    res.set_header("Content-Type", "application/json");

    // 解析 JSON
    const auto body = nlohmann::json::parse(req.body, nullptr, false);
    if (body.is_discarded())
    {
        res.status = 400;
        res.set_content(R"({"error":"无效的 JSON"})", "application/json");
        return;
    }

    // 调用登录逻辑
    const auto username = body.value("username", "");
    const auto pwd      = body.value("password", "");
    auto result = auth::login_by_password(conn, username, pwd);

    // 返回结果
    if (!result)
    {
        res.status = 401;
        res.set_content(R"({"error":"用户名或密码错误"})", "application/json");
        return;
    }
    nlohmann::json resp;
    resp["id"]       = result->id;
    resp["username"] = *result->username;
    res.set_content(resp.dump(), "application/json");
}

// ── 路由注册 ──

void setup_routes(httplib::Server& svr, pqxx::connection& conn)
{
    const std::string allowed = read_frontend_origin_or_exit();

    svr.Options("/api/.*",
        [allowed](const auto&, auto& res)
        {
            handle_cors(res, allowed);
        }
    );

    svr.Post("/api/login/key",
        [&conn, allowed](const auto& req, auto& res)
        {
            handle_login_key(req, res, conn, allowed);
        }
    );

    svr.Post("/api/login/password",
        [&conn, allowed](const auto& req, auto& res)
        {
            handle_login_password(req, res, conn, allowed);
        }
    );
}

} // namespace http
