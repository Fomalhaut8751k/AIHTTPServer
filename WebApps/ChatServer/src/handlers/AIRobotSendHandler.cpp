#include "../../include/handlers/AIRobotSendHandler.h"
#include <cstring>

void AIRobotSendHandler::handle(const http::HttpRequest& req, http::HttpResponse* resp)
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

        // 获取用户信息和消息信息
        int userId = std::stoi(session->getValue("userId"));
        json parsed = json::parse(req.getBody());
        std::string action = parsed["action"];
        /*
            action: isAI ? 'send_ai_message' : 'send_message',
            targetId: contact.id,
            targetName: contact.name,
            message: message,
            timestamp: Date.now()
        */

        assert(action == "send_ai_message");

        std::string strTargetAIRobotId = parsed["targetId"];
        int targetAIRobotId = std::stoi(strTargetAIRobotId);
        std::string targetAIRobotName = parsed["targetName"];
        std::string question = parsed["message"];

        int64_t questionTimestamp = parsed["timestamp"].get<int64_t>() / 1000; // 移除毫秒部分
        std::string api_key = findApiKeyforRobotId(targetAIRobotId);

        auto& AIHelperPtr = server_->chatInformation[userId][targetAIRobotId];
        // 把消息写入AIHelper的messages中:
        AIHelperPtr->addMessage(question, questionTimestamp);

        auto AIResp = AIHelperPtr->chat(userId);
        // auto AIResp = aiRobotResponse(userId, question, api_key);
        std::string answer = AIResp.first;
        int64_t answerTimestamp = AIResp.second;
        // 把消息写入AIHelper的messages中:
        AIHelperPtr->addMessage(answer, answerTimestamp);
        
        /*  两种时间戳的表示方法有区别？
            (gdb) p questionTimestamp
            $1 = 1766733965629
            (gdb) p answerTimestamp
            $2 = 1766733967
        */
        // std::string messageForUser = "user";
        // std::string messageForRobot = "robot";

        for(auto& item: AIHelperPtr->GetMessage()){
            std::cout << item.second << ": " << item.first << std::endl;
        }

        writeIntoTargetOfflineMessage(userId, targetAIRobotId, question, "user", questionTimestamp);
        writeIntoTargetOfflineMessage(userId, targetAIRobotId, answer, "robot", answerTimestamp);

        json successResp;
        successResp["status"] = true;
        successResp["answer"] = answer;
        std::string successBody = successResp.dump(4);

        resp->setStatusLine(req.getVersion(), http::HttpResponse::k200Ok, "OK");
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

// 找到对应的机器人(通过robotid找到对应的api-key)
std::string AIRobotSendHandler::findApiKeyforRobotId(const int& robotid)
{
    std::string sql = "SELECT apikey FROM AIRobot WHERE robotid = ?";
    sql::ResultSet* res = mysqlUtil_.executeQuery(sql, robotid);
    if(res->next()){
        std::string api_key = res->getString("apikey");
        return api_key;
    }
    // 如果查询结果为空，返回空字符串
    return "";
}

// 机器人回答问题
std::pair<std::string, int64_t> AIRobotSendHandler::aiRobotResponse(const int& userId,
                                                // const std::string& userName,
                                                const std::string& question, 
                                                const std::string& api_key
                                               )
{
    // AIHelper的生命周期？
    std::shared_ptr<AIHelper> AIHelperPtr = std::make_shared<AIHelper>(api_key);
    // AIHelperPtr->addMessage(userId, true, question);
    return AIHelperPtr->chat(userId);
}

// 写入离线消息
// bool AIRobotSendHandler::writeIntoTargetOfflineMessage(const int& userId, const int& robotId,
//                             const std::string& message, const std::string& source, int64_t timestamp)
// {
//     std::string sql = "INSERT INTO offlineAIRobotMessage (userid, robotid, message, source, created_at) VALUES (?, ?, ?, ?, FROM_UNIXTIME(?))";
//     int affectedRows = mysqlUtil_.executeUpdate(sql, userId, robotId, message, source, timestamp);
//     // std::string sql = "INSERT INTO offlineAIRobotMessage (userid, robotid, message, source, created_at) VALUES (1, 1, '你好呀！✨ 很高兴见到你！今天过得怎么样呀？希望你度过了愉快的一天。我随时准备好陪你聊天、帮你解决问题，或者就这样轻松愉快地闲聊一会儿。有什么想跟我分享的吗？ 🌟', 'user', FROM_UNIXTIME(1766820845))" ;
//     // int affectedRows = mysqlUtil_.executeUpdate(sql);
//     return affectedRows > 0;
// }

// 通过MQ写离线消息
bool AIRobotSendHandler::writeIntoTargetOfflineMessage(const int& userId, const int& robotId,
                             const std::string& message, const std::string& source, int64_t timestamp)
{
    // 寻找对应的AIHelper会话
    auto& it = server_->chatInformation[userId][robotId];
    if(!it){
        logger_->ERROR("Unable to find relevant AIHelper");
        return false;
    }
    try
    {
        it->pushMessageToMysql(userId, robotId, message, source, timestamp);
    }
    catch(const std::exception& e)
    {
        std::cerr << e.what() << '\n';
        return false;
    }
    return true;
}