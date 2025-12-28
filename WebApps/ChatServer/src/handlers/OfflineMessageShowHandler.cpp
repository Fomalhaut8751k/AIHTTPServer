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
        json parsed = json::parse(req.getBody());

        // 获取用户信息
        int userId = std::stoi(session->getValue("userId"));
        std::string friendIdStr = parsed["targetId"];
        int friendId = std::stoi(friendIdStr);
        
        // 存放离线消息的json串
        json successResp;
        successResp["message"] = json::array();
        successResp["status"] = getOfflineMessage(userId, friendId, successResp);

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

bool OfflineMessageShowHandler::getOfflineMessage(const int& userId, const int& friendId, json& js)
{
    std::string sql = "SELECT * FROM offlineFriendMessage WHERE ((userid = ? AND `from` = ?) OR (userid = ? AND `from` = ?)) ORDER BY created_at"; // 可以按时间排序
    sql::ResultSet* res = mysqlUtil_.executeQuery(sql, userId, friendId, friendId, userId);
    if(!res){
        logger_->WARN("Query failed or no results");
        return false;
    }
    // 找出所有以userId为发送方或者接收方的消息
    int index = 0;
    while(res->next())
    {
        int fromId = res->getInt("from");
        int toId = res->getInt("userid");

        std::string message = res->getString("message");
        std::string timestamp = res->getString("created_at");
        int OtherId = (fromId == userId ? toId : fromId);  // 找到对面的Id
        std::string OtherIdStr = std::to_string(OtherId);

        js["message"].push_back(
            {
                {"source", OtherId == fromId ? "friend" : "user"},  // 判断对方是否是发送方
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

/*
bool OfflineMessageShowHandler::getOfflineMessage(const int& userId, json& js)
{
    std::string sql = "SELECT from, message, create_at FROM offlineFriendMessage WHERE userid = ?";
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
        js["message_from"][index++] = {
            {"messageSource", messageSource}, 
            {"message", message},
            {"timestamp", timestamp}
        };
    }
    return true;
}

*/