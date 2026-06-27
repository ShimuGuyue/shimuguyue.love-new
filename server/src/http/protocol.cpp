/**
 * @file protocol.cpp
 * @brief HTTP 协议层实现
 *
 * @details
 * 简易 HTTP/1.1 协议实现，仅支持本项目所需的 POST 请求处理：
 * - 请求解析：从 socket 逐行读取请求行和头部，按 Content-Length 读取正文
 * - 响应构建：生成 JSON 响应（含 CORS 头 Access-Control-Allow-Origin）
 * - JSON 提取：从扁平的 JSON 字符串中按字段名提取引号内字符串值
 */

#include "http/protocol.h"

#include <sys/socket.h>
#include <unistd.h>

#include <sstream>

namespace http
{

/*
==================================================
  socket I/O — 按行 / 按字节读取
==================================================
*/

namespace
{

/**
 * @brief 从 socket 读取一行（支持 \\r\\n 和裸 \\n）
 *
 * @param fd   socket 描述符
 * @param line 输出行内容（不含换行符）
 * @return true 读到行；false 连接关闭或出错
 */
bool read_line(int fd, std::string& line)
{
    line.clear();
    char ch;
    while (true)
    {
        ssize_t n = recv(fd, &ch, 1, 0);
        if (n <= 0)
            return false;
        if (ch == '\n')
            return true;               // bare \n (Vite proxy)
        if (ch == '\r')
        {
            n = recv(fd, &ch, 1, MSG_PEEK);
            if (n > 0 && ch == '\n')
                recv(fd, &ch, 1, 0);   // consume \n of \r\n pair
            return true;
        }
        line.push_back(ch);
    }
}

/**
 * @brief 从 socket 读取指定字节数
 *
 * @param fd  socket 描述符
 * @param len 期望读取字节数
 * @return 读取到的字符串（可能短于 len）
 */
std::string read_bytes(int fd, std::size_t len)
{
    std::string result(len, '\0');
    std::size_t total{ 0 };
    while (total < len)
    {
        ssize_t n = recv(fd, result.data() + total, len - total, 0);
        if (n <= 0)
            break;
        total += static_cast<std::size_t>(n);
    }
    result.resize(total);
    return result;
}

} // namespace

/*
==================================================
  请求解析
==================================================
*/

Request parse_request(int fd)
{
    Request req;
    std::string request_line;
    if (!read_line(fd, request_line))
        return req;

    std::istringstream line_stream(request_line);
    line_stream >> req.method >> req.path;

    std::string header_line;
    while (read_line(fd, header_line) && !header_line.empty())
    {
        auto colon = header_line.find(':');
        if (colon != std::string::npos)
        {
            std::string key   = header_line.substr(0, colon);
            // HTTP header 名称大小写不敏感，统一转小写
            for (auto& c : key)
                if (c >= 'A' && c <= 'Z')
                    c += ('a' - 'A');
            std::string value = header_line.substr(colon + 1);
            std::size_t start{ 0 };
            while (start < value.size() && value[start] == ' ')
                ++start;
            req.headers[key] = value.substr(start);
        }
    }

    auto it = req.headers.find("content-length");
    if (it != req.headers.end())
    {
        int content_length = std::stoi(it->second);
        req.body = read_bytes(fd, static_cast<std::size_t>(content_length));
    }

    return req;
}

/*
==================================================
  响应构建
==================================================
*/

std::string build_response(int status_code, const std::string& body)
{
    std::string status_text;
    switch (status_code)
    {
    case 200: status_text = "OK"; break;
    case 400: status_text = "Bad Request"; break;
    case 401: status_text = "Unauthorized"; break;
    case 403: status_text = "Forbidden"; break;
    case 404: status_text = "Not Found"; break;
    case 500: status_text = "Internal Server Error"; break;
    default:  status_text = "Unknown"; break;
    }

    std::ostringstream response;
    response << "HTTP/1.1 " << status_code << " " << status_text << "\r\n"
             << "Content-Type: application/json; charset=utf-8\r\n"
             << "Content-Length: " << body.size() << "\r\n"
             << "Connection: close\r\n"
             << "Access-Control-Allow-Origin: *\r\n"
             << "\r\n"
             << body;
    return response.str();
}

/*
==================================================
  JSON 字段提取
==================================================
*/

std::string json_extract(const std::string& json, const std::string& field)
{
    std::string search = "\"" + field + "\"";
    auto pos = json.find(search);
    if (pos == std::string::npos)
        return {};

    auto colon = json.find(':', pos + search.size());
    if (colon == std::string::npos)
        return {};

    auto start_quote = json.find('"', colon + 1);
    if (start_quote == std::string::npos)
        return {};

    auto end_quote = json.find('"', start_quote + 1);
    if (end_quote == std::string::npos)
        return {};

    return json.substr(start_quote + 1, end_quote - start_quote - 1);
}

} // namespace http