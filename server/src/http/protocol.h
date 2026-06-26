/**
 * @file protocol.h
 * @brief HTTP 协议层 — 请求解析 / 响应构建 / JSON 提取
 */

#pragma once

#include <string>
#include <unordered_map>

namespace http
{

/**
 * @brief HTTP 请求结构
 */
struct Request
{
    std::string method;
    std::string path;
    std::unordered_map<std::string, std::string> headers;
    std::string body;
};

/**
 * @brief 从 socket 解析 HTTP 请求
 *
 * @param fd socket 描述符
 * @return 解析后的 Request；path 为空表示解析失败
 */
Request parse_request(int fd);

/**
 * @brief 构建 HTTP JSON 响应
 *
 * @param status_code HTTP 状态码
 * @param body        JSON 响应体
 * @return 完整 HTTP 响应字符串
 */
std::string build_response(int status_code, const std::string& body);

/**
 * @brief 从 JSON 字符串提取字段值
 *
 * @param json  JSON 字符串
 * @param field 字段名
 * @return 字段值；未找到返回空
 */
std::string json_extract(const std::string& json, const std::string& field);

} // namespace http
