#include "../../include/handlers/GetAIRobotListHandler.h"

void GetAIRobotListHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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

        // 存放好友信息的json串
        json successResp;
        successResp["contacts"] = json::array();
        successResp["status"] = getAIRobotList(successResp, userId);
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

bool GetAIRobotListHandler::getAIRobotList(json& js, int userid)
{
    std::string sql = "SELECT f.robotid, u.name FROM user2Robot f   \
                JOIN AIRobot u ON f.robotid = u.robotid WHERE f.userid = ?";
    sql::ResultSet* res = mysqlUtil_.executeQuery(sql, userid);
    if(!res)
    {
        logger_->WARN("Query failed or no results");
        return false;
    }
    int index = 0;
    while(res->next())
    {
        int robotid = res->getInt("robotid");
        std::string airobotname = res->getString("name");
        js["contacts"][index++] = {{"id", robotid}, {"name", airobotname}};
    }
    return true;
}