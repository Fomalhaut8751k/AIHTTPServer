#ifndef SEARCHHANDLER_H
#define SEARCHHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

class SearchHandler: public http::router::RouterHandler
{
public:
    explicit SearchHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resq) override;

private:
    bool isUserExist(const std::string& username);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif