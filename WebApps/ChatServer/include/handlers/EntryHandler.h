#ifndef ENTRYHANDLER_H
#define ENTRYHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

class EntryHandler: public http::router::RouterHandler
{
public:
    explicit EntryHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;
private:
    ChatServer* server_;
};

#endif