#ifndef OFFLINEMESSAGESHOWHANDLER_H
#define OFFLINEMESSAGESHOW_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

class OfflineMessageShowHandler: public http::router::RouterHandler
{
public:
    explicit OfflineMessageShowHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    bool getOfflineMessage(const int& userId, const int& friendId, json& js);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif