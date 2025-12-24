cmake_minimum_required(VERSION 3.10)
project(simple_server)

# 设置 C++ 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 在文件开头添加 OpenSSL 查找
find_package(OpenSSL REQUIRED)
find_package(CURL REQUIRED)

# ====== 新增：查找 SimpleAmqpClient 和依赖库 ======
# 手动配置 SimpleAmqpClient（因为它在 /usr/local/ 下）
set(SIMPLEAMQPCLIENT_INCLUDE_DIR "/usr/local/include")
set(SIMPLEAMQPCLIENT_LIB_DIR "/usr/local/lib")

# 查找 SimpleAmqpClient 库
find_library(SIMPLEAMQPCLIENT_LIB SimpleAmqpClient
    PATHS ${SIMPLEAMQPCLIENT_LIB_DIR}
    NO_DEFAULT_PATH
)

if(NOT SIMPLEAMQPCLIENT_LIB)
    find_library(SIMPLEAMQPCLIENT_LIB SimpleAmqpClient)
endif()

if(SIMPLEAMQPCLIENT_LIB)
    message(STATUS "Found SimpleAmqpClient: ${SIMPLEAMQPCLIENT_LIB}")
else()
    message(FATAL_ERROR "SimpleAmqpClient library not found!")
endif()

# 查找 rabbitmq-c（SimpleAmqpClient 的依赖）
find_library(RABBITMQ_LIB rabbitmq
    PATHS /usr/lib /usr/local/lib
)

if(RABBITMQ_LIB)
    message(STATUS "Found rabbitmq-c: ${RABBITMQ_LIB}")
else()
    message(FATAL_ERROR "rabbitmq-c library not found!")
endif()

# 查找 boost（SimpleAmqpClient 依赖 boost）
find_package(Boost REQUIRED COMPONENTS system thread)
# ====== 新增结束 ======

# 添加头文件路径
include_directories(
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
    ${PROJECT_SOURCE_DIR}/HttpServer/include
    # ${PROJECT_SOURCE_DIR}/WebApps/GomokuServer/include
    ${PROJECT_SOURCE_DIR}/WebApps/ChatServer/include
    ${PROJECT_SOURCE_DIR}/WebApps/ChatServer/include/AIUtil
    /usr/include/mysql-cppconn-8
    /usr/include/mysql
    ${OPENSSL_INCLUDE_DIR}
    ${CURL_INCLUDE_DIRS}
    ${SIMPLEAMQPCLIENT_INCLUDE_DIR}        # 新增
    ${Boost_INCLUDE_DIRS}                   # 新增
)

# 查找必要的库
find_library(MYSQLCPPCONN_LIBRARY
    NAMES mysqlcppconn mysqlcppconn8
    PATHS /usr/lib /usr/lib64 /usr/local/lib
)

# 添加所有源文件
file(GLOB_RECURSE HTTP_SERVER_SRC
    "${PROJECT_SOURCE_DIR}/HttpServer/src/*.cpp"
)

file(GLOB_RECURSE CHAT_SERVER_SRC
    # "${PROJECT_SOURCE_DIR}/WebApps/GomokuServer/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/WebApps/ChatServer/src/*.cpp"
)

file(GLOB_RECURSE AIUTIL_SERVER_SRC
    # "${PROJECT_SOURCE_DIR}/WebApps/GomokuServer/src/*.cpp"
    "${PROJECT_SOURCE_DIR}/WebApps/ChatServer/src/AIUtil/*.cpp"
)

# 添加主程序文件
# set(MAIN_SRC "${PROJECT_SOURCE_DIR}/WebApps/GomokuServer/src/main.cpp")
set(MAIN_SRC "${PROJECT_SOURCE_DIR}/WebApps/ChatServer/src/main.cpp")

# 如果主程序文件不存在，创建一个
if(NOT EXISTS ${MAIN_SRC})
    file(WRITE ${MAIN_SRC}
"#include \"../include/ChatServer.h\"
#include <iostream>

int main(int argc, char* argv[]) {
    try {
        ChatServer server;
        server.start();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << \"Error: \" << e.what() << std::endl;
        return 1;
    }
}
")
endif()

# 添加可执行文件
add_executable(chat_server
    ${MAIN_SRC}
    ${HTTP_SERVER_SRC}
    ${CHAT_SERVER_SRC}
    ${AIUTIL_SERVER_SRC}
)

# 链接必要的库
target_link_libraries(chat_server
    pthread
    # muduo_net
    # muduo_base
    mysqlcppconn
    mysqlclient
    ssl
    crypto
    mymuduo
    mylog
    ${CURL_LIBRARIES}
    ${SIMPLEAMQPCLIENT_LIB}    # 新增
    ${RABBITMQ_LIB}            # 新增
    ${Boost_LIBRARIES}         # 新增
)

# 设置运行时库搜索路径（如果需要）
set_target_properties(chat_server PROPERTIES
    INSTALL_RPATH "${SIMPLEAMQPCLIENT_LIB_DIR}"
)

# 打印调试信息
message(STATUS "Include directories:")
get_property(dirs DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY INCLUDE_DIRECTORIES)
foreach(dir ${dirs})
    message(STATUS "  ${dir}")
endforeach()

# message(STATUS "Main source file: ${MAIN_SRC}")
# message(STATUS "HTTP Server sources count: " ${#HTTP_SERVER_SRC})
# message(STATUS "Chat Server sources count: " ${#CHAT_SERVER_SRC})
# message(STATUS "AIUtil Server sources count: " ${#AIUTIL_SERVER_SRC})

# 打印链接的库
message(STATUS "Linking with:")
message(STATUS "  SimpleAmqpClient: ${SIMPLEAMQPCLIENT_LIB}")
message(STATUS "  rabbitmq-c: ${RABBITMQ_LIB}")
message(STATUS "  Boost: ${Boost_LIBRARIES}")
message(STATUS "  CURL: ${CURL_LIBRARIES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O0")