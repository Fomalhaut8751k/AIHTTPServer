#ifndef SEARCHAIROBOTEXISTHANDLER_H
#define SEARCHAIROBOTEXISTHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

class SearchAIRobotExistHandler: public http::router::RouterHandler
{
public:
    explicit SearchAIRobotExistHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resq) override;
    
private:
    int queryAIRobotId(const std::string &robotname);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif