#include "../../include/handlers/OfflineMessageShowHandler.h"

void OfflineMessageShowHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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

        // 获取用户信息
        int userId = std::stoi(session->getValue("userId"));
        // 从数据库中查询自己的离线消息
        // 存放离线消息的json串
        json successResp;
        successResp["message"] = json::array();
        successResp["status"] = getOfflineMessage(userId, successResp);

        // if (successResp["message"].is_array()) {
        //     for (auto it = successResp["message"].begin(); 
        //         it != successResp["message"].end(); ++it) {
                
        //         const auto& item = *it;
        //         std::cout << item["message"] << std::endl;
        //         std::cout << item["messageSource"] << std::endl;
        //     }
        // }

        std::string successBody = successResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");   // 版本，状态码，...
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(successBody.size());
        resp->setBody(successBody);
        return;

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


bool OfflineMessageShowHandler::getOfflineMessage(const int& userId, json& js)
{
    std::string sql = "SELECT * FROM offlineFriendMessage WHERE userid = ?";
    sql::ResultSet* res = mysqlUtil_.executeQuery(sql, userId);
    if(!res){
        logger_->WARN("Query failed or no results");
        return false;
    }
    int index = 0;
    while(res->next())
    {
        int messageSource = res->getInt("from");
        std::string message = res->getString("message");
        std::string timestamp = res->getString("created_at");
        js["message"][index++] = {
            {"messageSource", messageSource}, 
            {"message", message},
            {"timestamp", timestamp}
        };
    }
    /*
        {
            "status": true,
            "message": [
                {
                    "messageSource": "2",
                    "message": "pdcHelloWorld",
                    "timestamp": "2025-12-22 14:38:37"
                },
                {
                    "messageSource": "2",
                    "message": "pdcHelloWorld",
                    "timestamp": "2025-12-22 14:38:37"
                }
            ]
        }
    */
    // 读取后的离线消息要删除
    sql = "DELETE FROM offlineFriendMessage WHERE userid = ?";
    int affect = mysqlUtil_.executeUpdate(sql, userId);

    return true;
}