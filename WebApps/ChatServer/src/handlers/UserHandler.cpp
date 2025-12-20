#include "../../include/handlers/UserHandler.h"

void UserHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
{
    // Json 解析使用try catch捕获异常
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
        std::string username = session->getValue("username");

        std::string reqFile("../WebApps/ChatServer/resource/user.html");
        FileUtil fileOperater(reqFile);
        if(!fileOperater.isValid())
        {
            logger_->WARN(reqFile + " not exist.");
            fileOperater.resetDefaultFile();
        }

        std::vector<char> buffer(fileOperater.size());
        fileOperater.readFile(buffer);  // 读出文件数据
        std::string htmlContent(buffer.data(), buffer.size());

        // 在HTML内容中插入userId
        size_t headEnd = htmlContent.find("</head>");
        if(headEnd != std::string::npos)
        {
            std::string script = "<script>const userId = '" + std::to_string(userId) + "';</script>";
            htmlContent.insert(headEnd, script);
        }

        size_t pos = htmlContent.find(R"(<div class="app-name">WeChat</div>)");
        pos = htmlContent.find("WeChat", pos);

        // 替换模板中的占位符
        if(pos != std::string::npos)
        {
            htmlContent.replace(pos, 6, "Hello, " + username);  // 11是"{{username}}"的长度
        }

        resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");
        resp->setCloseConnection(false);
        resp->setContentType("text/html");
        resp->setContentLength(htmlContent.size());
        resp->setBody(htmlContent);
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