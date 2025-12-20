#ifndef FRIENDHANDLER_H
#define FRIENDHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

class FriendHandler: public http::router::RouterHandler
{
public:
    explicit FriendHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    ChatServer* server_;
};

#endif