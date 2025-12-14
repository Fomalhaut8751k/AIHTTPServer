#ifndef LOGINHANDLER_H
#define LOGINHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../GomokuServer.h"
#include "../../../../HttpServer/include/utils/JsonUtil.h"


class LoginHandler: public http::router::RouterHandler
{
public:
    explicit LoginHandler(GomokuServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    int queryUserId(const std::string& username, const std::string& password);

    GomokuServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif