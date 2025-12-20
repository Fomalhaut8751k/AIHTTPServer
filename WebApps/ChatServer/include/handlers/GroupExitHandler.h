#ifndef GROUPEXITHANDLER_H
#define GROUPEXITHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

class GroupExitHandler: public http::router::RouterHandler
{
public:
    explicit GroupExitHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    ChatServer* server_;
};

#endif