#ifndef OFFLINEAIMESSAGESHOWHANDLER_H
#define OFFLINEAIMESSAGESHOWHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

class OfflineAIMessageShowHandler: public http::router::RouterHandler
{
public:
    explicit OfflineAIMessageShowHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    bool getOfflineMessage(const int& userId, const int& robotId, json& js);
    bool getApplicationIdForRAG(const int& robotid, std::string& aid);

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};

#endif