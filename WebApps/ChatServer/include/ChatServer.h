#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <atomic>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <mutex>

#include "../../../HttpServer/include/http/HttpServer.h"
#include "../../../HttpServer/include/utils/MysqlUtil.h"
#include "../../../HttpServer/include/utils/FileUtil.h"
#include "../../../HttpServer/include/utils/JsonUtil.h"

class ChatServer
{
public:
    ChatServer(int port, const std::string& name, 
                TcpServer::Option option = TcpServer::kNoReusePort);
    
    void setThreadNum(int numThreads);
    void start();

private:
    void initialize();
    void initializeSession();
    void initializeRouter();
    void initializeMiddleWare();

    void setSessionManager(std::unique_ptr<http::session::SessionManager> manager);

    http::session::SessionManager* getSessionManager() const;

    

    http::HttpServer httpServer_;
    http::MysqlUtil mysqlUtil_;
};

#endif