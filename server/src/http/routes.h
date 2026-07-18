/**
 * @file http/routes.h
 * @brief HTTP 路由注册
 */
#pragma once

#include <pqxx/pqxx>

#define CPPHTTPLIB_OPENSSL_SUPPORT 0
#include <httplib.h>

namespace http {

/**
 * @brief 从环境变量 SERVER_PORT 读取端口号。
 *        未设置或无效则打印错误并调用 std::exit(1)。
 * @return 有效的端口号。
 */
[[nodiscard]] auto read_port_or_exit() -> int;

/**
 * @brief 从环境变量 FRONTEND_ORIGIN 读取允许的前端地址。
 *        未设置则打印错误并调用 std::exit(1)。
 * @return 前端 origin（如 "http://localhost:5173"）。
 */
[[nodiscard]] auto read_frontend_origin_or_exit() -> std::string;

/**
 * @brief 从环境变量 SERVER_HOST 读取监听地址。
 *        未设置则打印错误并调用 std::exit(1)。
 * @return 监听地址（如 "127.0.0.1" 或 "0.0.0.0"）。
 */
[[nodiscard]] auto read_host_or_exit() -> std::string;

/**
 * @brief 注册所有 API 路由到 HTTP 服务器。
 * @param svr  httplib::Server 实例。
 * @param conn 数据库连接。
 */
void setup_routes(httplib::Server& svr, pqxx::connection& conn);

} // namespace http
