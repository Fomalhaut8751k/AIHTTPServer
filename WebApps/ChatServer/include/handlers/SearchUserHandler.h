#ifndef SEARCHUSERHANDLER_H
#define SEARCHUSERHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

class SearchUserHandler: public http::router::RouterHandler
{
public:
    explicit SearchUserHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resq) override;

private:
    bool isUserExist(const std::string& username);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif