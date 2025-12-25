#ifndef SEARCHUSEREXISTHANDLER_H
#define SEARCHUSEREXISTHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"
#include "../ChatServer.h"

class SearchUserExistHandler: public http::router::RouterHandler
{
public:
    explicit SearchUserExistHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resq) override;

private:
    // bool isUserExist(const std::string& username);
    int queryUserId(const std::string &username);
    
    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};


#endif