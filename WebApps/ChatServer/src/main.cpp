#include <string>
#include <iostream>
#include "mymuduo/TcpServer.h"
#include "mymuduo/Alogger.h"
#include "mymuduo/EventLoop.h"

#include "../include/ChatServer.h"

const std::string RABBITMQ_HOST = "localhost";
const std::string QUEUE_NAME = "sql_queue";
const int THREAD_NUM = 2;

void executeMysql(const std::string sql){
    http::MysqlUtil mysqlUtil_;
    mysqlUtil_.executeUpdate(sql);
}

int main(int argc, char* argv[])
{
    logger_->INFO("pid = " + getpid());

    std::string serverName = "HttpServer";
    int port = 8081;

    // 参数解析
    int opt;
    const char* str = "p:";
    while((opt = getopt(argc, argv, str)) != -1)
    {
        switch(opt)
        {
            case 'p':
            {
                port = atoi(optarg);
                break;
            }
            default:
                break;
        }
    }

    std::cout << "port: " << port << std::endl;
    
    ChatServer server(port, serverName);
    server.setThreadNum(4);

    RabbitMQThreadPool pool(RABBITMQ_HOST, QUEUE_NAME, THREAD_NUM, executeMysql);
    pool.start();

    server.start();
}