/**
 * @file connection_pool.h
 * @brief 数据库连接管理 — 单连接版
 *
 * @details
 * 对个人网站项目，单连接完全足够。保留 borrow() 接口是为了将来平滑
 * 升级到真正的连接池（只需改内部实现，不改调用方）。
 *
 * pqxx 内部抛出的异常在 connect() 边界处被捕获，
 * 并转换为错误状态（last_error / is_connected）。
 *
 * RAII 保证：
 * - ConnectionPool 析构时关闭连接
 * - borrow() 返回的事务析构时自动 ROLLBACK（如果没 commit）
 */

#pragma once
#include <pqxx/pqxx>
#include <string>
#include <memory>

/**
 * @brief 数据库连接管理器
 *
 * 持有单个 pqxx::connection，对外提供事务借用接口。
 * 当前为单连接实现，但接口设计预留了升级到连接池的空间。
 * 所有可能失败的操作通过返回值 + is_connected() / last_error() 报告
 */
class ConnectionPool
{
private:
    std::string connect_str_;                       ///< 连接字符串（libpq 格式或 service= 引用）
    std::unique_ptr<pqxx::connection> connection_;  ///< 底层连接，未连接时为 nullptr
    std::string last_error_;                        ///< 最近一次错误描述

public:
    /**
     * @brief 构造函数 — 仅保存连接字符串，不建立连接
     *
     * 调用 connect() 才会实际连接数据库。
     *
     * @param connect_str PostgreSQL 连接字符串（libpq 格式或 service= 引用）
     *
     * @code
     * ConnectionPool pool{"service=love"};
     * if (!pool.connect()) {
     *     std::cerr << pool.last_error() << '\n';
     *     return 1;
     * }
     * @endcode
     */
    explicit ConnectionPool(std::string connect_str);

    /**
     * @brief 析构 — 关闭底层连接，释放服务端资源
     */
    ~ConnectionPool();

    /// 一个连接不能被两个对象同时持有
    ConnectionPool(const ConnectionPool&) = delete;
    ConnectionPool& operator=(const ConnectionPool&) = delete;

    /// 支持移动
    ConnectionPool(ConnectionPool&&) noexcept = default;
    ConnectionPool& operator=(ConnectionPool&&) noexcept = default;

public:
    /**
     * @brief 尝试连接数据库
     *
     * 内部捕获 pqxx 异常并转换为错误状态。
     * 重复调用安全：如果已连接，先断开再重连。
     *
     * @return true  连接成功
     * @return false 连接失败，调用 last_error() 获取详情
     */
    bool connect();

    /**
     * @brief 查询连接是否有效
     *
     * @return true  已连接且可用
     * @return false 未连接或已断开
     */
    bool is_connected() const;

    /**
     * @brief 获取最后一次错误的描述
     *
     * @return 错误信息字符串；从未出错时为空。
     */
    const std::string& last_error() const;

    /**
     * @brief 借出一个事务
     *
     * 返回 pqxx::work（read-committed 隔离级别的读写事务）。
     * 调用者用完必须调用 .commit()，否则析构时自动 ROLLBACK。
     *
     * @return 指向新建事务的 unique_ptr；未连接时返回 nullptr
     *
     * @note 调用者持有事务期间不要再调用 borrow()，
     *       因为底层只有一个连接，同一时刻只能有一个活跃事务。
     *       这是单连接方案的已知限制，后续升级到连接池后解除。
     */
   std::unique_ptr<pqxx::work> borrow();
};
