#ifndef GETFRIENDLISTHANDLER_H
#define GETFRIENDLISTHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

class GetFriendListHandler: public http::router::RouterHandler
{
public:
    explicit GetFriendListHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp);

private:
    bool getFriendList(json& js, int userid);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif