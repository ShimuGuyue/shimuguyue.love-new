## @file CMakeLists.txt
## @brief shimuguyue.love 服务端构建脚本
##
## @details
## 编译器: GCC ≥12 或 Clang ≥16（需要完整 C++23 支持）
## 依赖:   libpqxx ≥7.7（PostgreSQL 官方 C++ 客户端库）
## 产物:   build/server（可执行文件）

## @brief CMake 版本底线
cmake_minimum_required(VERSION 3.22)

## @brief 项目声明
##
## - @c ${PROJECT_NAME} = shimuguyue.love-server
## - 检测 C++ 编译器
## - @c ${CMAKE_SOURCE_DIR} = 当前 CMakeLists.txt 所在目录（隐式创建）
project(shimuguyue.love-server LANGUAGES CXX)

## @brief C++ 标准
set(CMAKE_CXX_STANDARD 23)

## @brief 强制版本支持
set(CMAKE_CXX_STANDARD_REQUIRED ON)

## 禁用编译器专属扩展（如 GNU）
set(CMAKE_CXX_EXTENSIONS OFF)

## @brief 查找 libpqxx（通过 pkg-config 桥接）
##
## @details
## 系统的 libpqxx 没有 CMake config 文件（libpqxxConfig.cmake），
## 但安装了 .pc 文件。用 pkg-config 桥接。
##
## @note 若 libpqxx 提供了 CMake config，可以启用下面的 find_package 替代：
## @code
## find_package(libpqxx REQUIRED)
## target_link_libraries(server PRIVATE libpqxx::pqxx)
## @endcode
find_package(PkgConfig REQUIRED)
pkg_check_modules(PQXX REQUIRED IMPORTED_TARGET libpqxx)
pkg_check_modules(SODIUM REQUIRED IMPORTED_TARGET libsodium)

## @brief 源文件清单
add_executable(server
    src/main.cpp
    src/crypto/hasher.cpp
    src/db/connection_pool.cpp
    src/http/protocol.cpp
    src/http/handlers.cpp
    src/http/blog_import.cpp
    src/http/blog_query.cpp
    src/http/login_handler.cpp
    src/http/server.cpp
)

## @brief 链接 PostgreSQL 库
target_link_libraries(server PRIVATE PkgConfig::PQXX PkgConfig::SODIUM)

## @brief 设置头文件搜索路径
target_include_directories(server PRIVATE ${CMAKE_SOURCE_DIR}/src)
