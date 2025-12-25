#include "../../include/handlers/SearchAIRobotExistHandler.h"

void SearchAIRobotExistHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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
        assert(parsed["action"] == "search_ai");
        std::string robotname = parsed["robotname"];

        // 验证查找的机器人是否存在
        int robotId = queryAIRobotId(robotname);
        if(robotId != -1)
        {
            // 找到对应的机器人
            json successResp;
            successResp["status"] = true;
            successResp["targetId"] = std::to_string(robotId);
            std::string successBody = successResp.dump(4);

            resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");
            resp->setCloseConnection(false);
            resp->setContentType("application/json");
            resp->setContentLength(successBody.size());
            resp->setBody(successBody);
        }
        else
        {   // 找不到对应的用户
            json failureResp;
            failureResp["status"] = false;
            std::string failureBody = failureResp.dump(4);

            resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");
            resp->setCloseConnection(false);
            resp->setContentType("application/json");
            resp->setContentLength(failureBody.size());
            resp->setBody(failureBody);
        }
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
    }
    
}

int SearchAIRobotExistHandler::queryAIRobotId(const std::string &robotname)
{
    // 前端用户传来账号密码，查找数据库是否有该账号密码
    // 使用预处理语句，防止sql注入
    std::string sql = "SELECT robotid FROM AIRobot WHERE name = ?";
    sql::ResultSet* res = mysqlUtil_.executeQuery(sql, robotname);
    if(res->next())
    {
        int id = res->getInt("robotid");
        return id;
    }
    // 如果查询结果为空，返回-1
    return -1;
}