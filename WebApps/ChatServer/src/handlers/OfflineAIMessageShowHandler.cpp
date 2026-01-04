#include "../../include/handlers/OfflineAIMessageShowHandler.h"

void OfflineAIMessageShowHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    try
    {
        // 检查用户是否登录
        auto session = server_->getSessionManager()->getSession(req, resp);
        logger_->INFO("session->getValue(\"isLoggedIn\") = " + session->getValue("isLoggedIn"));
        if(session->getValue("isLoggedIn") != "true")
        {
            // 用户未登录，返回未授权错误
            json errorResp;
            errorResp["status"] = "error";
            errorResp["message"] = "Unauthorized";
            std::string errorBody = errorResp.dump(4);

            server_->packageResp(req.getVersion(), http::HttpResponse::k401Unauthorized,
                            "Unauthorized", true, "application/json", errorBody.size(),
                            errorBody, resp
            );
            return;
        }
        json parsed = json::parse(req.getBody());

        // 获取用户信息
        int userId = std::stoi(session->getValue("userId"));
        std::string robotIdStr = parsed["targetId"];
        int robotId = std::stoi(robotIdStr);

        json successResp;
        successResp["message"] = json::array();
        successResp["status"] = getOfflineMessage(userId, robotId, successResp);

        std::string successBody = successResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");   // 版本，状态码，...
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(successBody.size());
        resp->setBody(successBody);
    }
    catch(const std::exception& e)
    {
        // 捕获异常，返回错误信息
        json failureResp;
        failureResp["status"] = "error";
        failureResp["message"] = e.what();
        std::string failureBody = failureResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::k400BadRequest, "Bad Request");
        resp->setCloseConnection(true);
        resp->setContentType("application/json");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);

        return;
    }
}

bool OfflineAIMessageShowHandler::getOfflineMessage(const int& userId, const int& robotId, json& js)
{
    // AIHelper的创建还需要对应的apiKey
    std::string sql = R"(
        SELECT o.*, r.apikey, r.strategyType
        FROM offlineAIRobotMessage o 
        JOIN AIRobot r ON o.robotid = r.robotid 
        WHERE o.userid = ? AND o.robotid = ? ORDER BY created_at ASC
    )";
    sql::ResultSet* res = mysqlUtil_.executeQuery(sql, userId, robotId);
    if(!res){
        logger_->WARN("Query failed or no results");
        return false;
    }
    while(res->next())
    {
        // int userid = res->getInt("userid");
        // int robotid = res->getInt("robotid");

        std::string message = res->getString("message");
        std::string source = res->getString("source");
        std::string timestamp = res->getString("created_at");
        long long ts = res->getInt64("created_at");
        std::string apikey = res->getString("apikey");
        int strategyType = res->getInt("strategyType");

        /* 此会话非彼会话，指的就是用户和AI的聊天

            1. std::unordered_map<int, std::unordered_map<int, std::shared_ptr<AIHelper>>> chatInformation;
            2. 因为是unordered_map，因此chatInformation[userId]不存在，也会创建一个空的
        */
        auto& userSessionsMap = server_->chatInformation[userId]; // user会话map，和若干个AI的会话
        std::shared_ptr<AIHelper> helper;
        auto itSession = userSessionsMap.find(robotId);  // user和某个robot的会话
        if(itSession == userSessionsMap.end())
        {   // 如果没有这个会话
            // helper = std::make_shared<AIHelper>(apikey);
            helper = std::make_shared<AIHelper>(strategyType);
            userSessionsMap[robotId] = helper;
        }
        else
        {
            helper = itSession->second;
        }
        // 存储到服务器的内存当中
        helper->restoreMessage(message, timestamp);

        js["message"].push_back(
            {
                {"source", source},
                {"message", message},
                {"timestamp", timestamp}
            }
        );
    }
    // auto& it = server_->chatInformation[userId][robotId];
    // for(auto& item : it->GetMessage()){
    //     std::cout << item.second << ": " << "pdcHelloWorld" << std::endl;
    // }
    return true;
}

/*
        {
            "status": true,
            "message": [
                {
                    "source": "user",
                    "message": "pdcHelloWorld",
                    "timestamp": "2025-12-22 14:38:37"
                },
                {
                    "source": "robot",
                    "message": "pdcHelloWorld",
                    "timestamp": "2025-12-22 14:38:37"
                }
            ]
        }
*/