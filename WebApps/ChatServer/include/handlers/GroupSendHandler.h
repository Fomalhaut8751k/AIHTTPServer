#ifndef GROUPSENDHANDLER_H
#define GROUPSENDHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

class GroupSendHandler: public http::router::RouterHandler
{
public:
    explicit GroupSendHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    ChatServer* server_;
};

#endif