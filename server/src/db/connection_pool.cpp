/**
 * @file connection_pool.cpp
 * @brief connection_pool.h 连接管理实现
 *
 * pqxx 库内部使用异常（如 pqxx::sql_error），本项目在边界处捕获，
 * 转换为错误状态并返回错误码，不向调用方传播异常。
 */

#include "db/connection_pool.h"

ConnectionPool::ConnectionPool(std::string connect_str)
    : connect_str_(std::move(connect_str))  //!< 只保存字符串，不做连接
{ }

ConnectionPool::~ConnectionPool() = default;

bool ConnectionPool::connect()
{
    // 尝试建立连接 —— 项目中 pqxx 异常的边界
    try
    {
        connection_ = std::make_unique<pqxx::connection>(connect_str_);
        last_error_.clear();
        return true;
    }
    catch (const pqxx::sql_error& e)
    {
        connection_.reset();
        last_error_ = e.what();
        return false;
    }
    catch (const std::exception& e)
    {
        connection_.reset();
        last_error_ = e.what();
        return false;
    }
}

bool ConnectionPool::is_connected() const
{
    return connection_ != nullptr && connection_->is_open();
}

const std::string& ConnectionPool::last_error() const
{
    return last_error_;
}

std::unique_ptr<pqxx::work> ConnectionPool::borrow()
{
    if (!is_connected())
    {
        last_error_ = "borrow() called while not connected";
        return nullptr;
    }
    return std::make_unique<pqxx::work>(*connection_);
}
