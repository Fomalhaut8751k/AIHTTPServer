#ifndef FRIENDEXITHANDLER_H
#define FRIENDEXITHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

class FriendExitHandler: public http::router::RouterHandler
{
public:
    explicit FriendExitHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    ChatServer* server_;
};

#endif