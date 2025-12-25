#include "../../include/handlers/AIRobotAddHandler.h"
#include <string>

void AIRobotAddHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    std::shared_ptr<http::session::Session> session = nullptr;
    try
    {
        // 检查用户是否登录
        session = server_->getSessionManager()->getSession(req, resp);
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
        std::string strTargetAIId = parsed["targetAIId"];
        int robotId = std::stoi(strTargetAIId);
        int userId = std::stoi(session->getValue("userId"));

        if(addAIRobot(userId, robotId)){
            json successResp;
            successResp["status"] = true;

            std::string successBody = successResp.dump(4);

            resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");   // 版本，状态码，...
            resp->setCloseConnection(false);
            resp->setContentType("application/json");
            resp->setContentLength(successBody.size());
            resp->setBody(successBody);
            return;
        }
        else{
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
    }
    
}

bool AIRobotAddHandler::addAIRobot(const int& myId, const int& robotId)
{
    std::string sql = "INSERT INTO user2Robot (userid, robotid) VALUES (?, ?)";
    int ret = mysqlUtil_.executeUpdate(sql, myId, robotId);
    if(ret > 0){
        return true;
    }
    return false;
}