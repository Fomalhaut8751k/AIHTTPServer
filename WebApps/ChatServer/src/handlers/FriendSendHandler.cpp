#include "../../include/handlers/FriendSendHandler.h"

void FriendSendHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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
        json parsed = json::parse(req.getBody());
        std::string action = parsed["action"];
        
        std::string strTargetUserId = parsed["targetUserId"];  
        int targetUserId = std::stoi(strTargetUserId);  // 接收方的id

        std::string strTargetUserName = parsed["targetUsername"];  // 接收方的名称
        
        std::string message = parsed["message"];  // 发送给对方的消息

        int64_t timestamp = parsed["timestamp"].get<int64_t>();  

        // 发送给好友（看似发送，实则写入数据库...）
        bool res = writeIntoTargetOfflineMessage(userId, targetUserId, message, timestamp);
        if(res) // 如果写入数据库成功
        {
            // 显示到自己和好友的聊天框上
            json successResp;
            successResp["status"] = true;
            successResp["message"] = message;
            std::string successBody = successResp.dump(4);

            resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");
            resp->setCloseConnection(false);
            resp->setContentType("application/json");
            resp->setContentLength(successBody.size());
            resp->setBody(successBody);

            return;
        }

        json failureResp;
        failureResp["status"] = false;
        std::string failureBody = failureResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("application/json");
        resp->setContentLength(failureBody.size());
        resp->setBody(failureBody);

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

bool FriendSendHandler::writeIntoTargetOfflineMessage(const int& myId, const int& targetId, const std::string& message, int64_t timestamp)
{
    std::string sql = "INSERT INTO offlineFriendMessage (userid, `from`, message, created_at) VALUES (?, ?, ?, FROM_UNIXTIME(?))";
    int affectedRows = mysqlUtil_.executeUpdate(sql, targetId, myId, message, timestamp);
    return affectedRows > 0;
}