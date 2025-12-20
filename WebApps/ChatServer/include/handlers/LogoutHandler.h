#ifndef LOGOUTHANDLER_H
#define LOGOUTHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"
#include "../../../../HttpServer/include/utils/JsonUtil.h"


class LogoutHandler: public http::router::RouterHandler
{
public:
    explicit LogoutHandler(ChatServer* server): server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    ChatServer* server_;
};

#endif