#ifndef AIROBOTSENDHANDLER_H
#define AIROBOTSENDHANDLER_H

#include "../../../../HttpServer/include/router/RouterHandler.h"
#include "../ChatServer.h"

class AIRobotSendHandler: public http::router::RouterHandler
{
public:
    explicit AIRobotSendHandler(ChatServer* server): server_(server){}

    void handle(const http::HttpRequest& req, http::HttpResponse* resp) override;

private:
    // 找到对应的机器人(通过robotid找到对应的api-key)
    std::string findApiKeyforRobotId(const int& robotid); 

    // 找到对应的机器人对应的策略(通过robotid)
    int findStrategyforRobotId(const int& robotid);

    // 找到对应的机器人对应的application(通过robotid)
    bool getApplicationIdForRAG(const int& robotid, std::string& aid);

    // 机器人回答问题
    std::pair<std::string, int64_t> aiRobotResponse(const int& userid,
                                                    // const std::string& username,
                                                    const std::string& question, 
                                                    const std::string& api_key
                                                   );

    // 写入离线消息
    bool writeIntoTargetOfflineMessage(const int& userId, 
                                       const int& robotId,
                                       const std::string& message,
                                       const std::string& source, 
                                       int64_t timestamp
                                      );

    ChatServer* server_;
    http::MysqlUtil mysqlUtil_;
};


#endif