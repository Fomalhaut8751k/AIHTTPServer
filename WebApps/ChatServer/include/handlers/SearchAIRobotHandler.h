#ifndef SEARCHAIROBOTHANDLER_H
#define SEARCHAIROBOTHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

class SearchAIRobotHandler: public http::router::RouterHandler
{
public:
    explicit SearchAIRobotHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resq) override;

private:
    // bool isRobotExist(const std::string& robotname);
    // int queryRobotId(const std::string& robotname);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif