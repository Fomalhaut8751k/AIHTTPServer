#ifndef GETAIROBOTLISTHANDLER_H
#define GETAIROBOTLISTHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

class GetAIRobotListHandler: public http::router::RouterHandler
{
public:
    explicit GetAIRobotListHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp);

private:
    bool getAIRobotList(json& js, int userid);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif