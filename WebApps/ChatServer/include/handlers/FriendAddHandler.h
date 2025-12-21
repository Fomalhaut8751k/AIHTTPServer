#ifndef FRIENDADDHANDLER_H
#define FRIENDADDHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

class FriendAddHandler: public http::router::RouterHandler
{
public:
    explicit FriendAddHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp);

private:
    bool addFriend(const int& myId, const int& friendId);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif