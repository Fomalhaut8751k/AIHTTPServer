#include "../../include/handlers/GetFriendListHandler.h"

void GetFriendListHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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
        successResp["status"] = getFriendList(successResp, userId);
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

bool GetFriendListHandler::getFriendList(json& js, int userid)
{
    std::string sql = "SELECT f.friendid, u.name FROM Friend f   \
                JOIN User u ON f.friendid = u.id WHERE f.userid = ?";
    sql::ResultSet* res = mysqlUtil_.executeQuery(sql, userid);
    if(!res)
    {
        logger_->WARN("Query failed or no results");
        return false;
    }
    int index = 0;
    while(res->next())
    {
        int friendid = res->getInt("friendid");
        std::string friendname = res->getString("name");
        js["contacts"][index++] = {{"id", friendid}, {"name", friendname}};
    }
    return true;
}

