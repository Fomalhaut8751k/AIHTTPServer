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
    std::string sql = "SELECT * FROM offlineAIRobotMessage WHERE userid = ? AND robotid = ?";
    sql::ResultSet* res = mysqlUtil_.executeQuery(sql, userId, robotId);
    if(!res){
        logger_->WARN("Query failed or no results");
        return false;
    }
    while(res->next())
    {
        int userid = res->getInt("userid");
        int robotid = res->getInt("robotid");

        std::string message = res->getString("message");
        std::string source = res->getString("source");
        std::string timestamp = res->getString("created_at");

        js["message"].push_back(
            {
                {"source", source},
                {"message", message},
                {"timestamp", timestamp}
            }
        );
    }

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