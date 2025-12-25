#ifndef AIROBOTADDHANDLER_H
#define AIROBOTADDHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

class AIRobotAddHandler: public http::router::RouterHandler
{
public:
    explicit AIRobotAddHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp);

private:
    bool addAIRobot(const int& myId, const int& robotId);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif