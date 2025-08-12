cmake_minimum_required(VERSION 3.16)

project(TradingTimeCounterProject 
    VERSION 1.0.0 
    LANGUAGES CXX
    DESCRIPTION "Trading Time Counter - Cross-platform countdown timer for traders"
)

# 全局设置
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# 设置全局输出目录
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

# 选项配置
option(BUILD_TESTS "Build test programs" OFF)
option(BUILD_EXAMPLES "Build example programs" OFF)
option(ENABLE_STATIC_ANALYSIS "Enable static analysis tools" OFF)

# 添加子项目
add_subdirectory(tradingTimeCounter)

# 显示配置信息
message(STATUS "=== Project Configuration ===")
message(STATUS "Project: ${PROJECT_NAME}")
message(STATUS "Version: ${PROJECT_VERSION}")
message(STATUS "Build Type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Install Prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "==============================")
