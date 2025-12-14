#ifndef AIGAMESTARTHANDLER_H
#define AIGAMESTARTHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../GomokuServer.h"

class AiGameStartHandler: public http::router::RouterHandler
{
public:
    explicit AiGameStartHandler(GomokuServer* server): server_(server) {}

    void handle(const http::HttpRequest& req, http::HttpResponse* resq) override;
private:
    GomokuServer* server_;
};

#endif