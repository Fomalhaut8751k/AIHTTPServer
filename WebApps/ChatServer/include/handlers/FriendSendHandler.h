#ifndef FRIENDSENDHANDLER_H
#define FRIENDSENDHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

class FriendSendHandler: public http::router::RouterHandler
{
public:
    explicit FriendSendHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
    
private:
    bool writeIntoTargetOfflineMessage(const int& myId, const int& targetId, const std::string& message, int64_t timestamp);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif