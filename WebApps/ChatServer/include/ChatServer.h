#ifndef CHATSERVER_H
#define CHATSERVER_H

#include <atomic>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <mutex>

#include "../../../HttpServer/include/http/HttpServer.h"
#include "../../../HttpServer/include/utils/MysqlUtil.h"
#include "../../../HttpServer/include/utils/FileUtil.h"
#include "../../../HttpServer/include/utils/JsonUtil.h"

#include "AIUtil/AIHelper.h"
#include "AIUtil/MQManager.h"

class EntryHandler;
class LoginHandler;
class LogoutHandler;

class RegisterHandler;

class UserHandler;

class SearchUserHandler;
class SearchUserExistHandler;
class FriendAddHandler;
class GetFriendListHandler;

class FriendSendHandler;

class SearchAIRobotHandler;
class SearchAIRobotExistHandler;
class AIRobotAddHandler;
class GetAIRobotListHandler;

class AIRobotSendHandler;

class GroupHandler;
class GroupSendHandler;
class GroupExitHandler;

class OfflineMessageShowHandler;
class OfflineAIMessageShowHandler;


class ChatServer
{
public:
    ChatServer(int port, const std::string& name, 
                TcpServer::Option option = TcpServer::kNoReusePort);
    
    void setThreadNum(int numThreads);
    void start();

    static std::string registerInvitationCode;

private:
    void initialize();
    void initializeSession();
    void initializeRouter();
    void initializeMiddleWare();

    void setSessionManager(std::unique_ptr<http::session::SessionManager> manager);

    http::session::SessionManager* getSessionManager() const;

    void packageResp(const std::string& version, http::HttpResponse::HttpStatusCode statusCode, 
                    const std::string& statusMsg, bool close, const std::string& contentType,
                    int contentLen, const std::string& body, http::HttpResponse* resp);

    friend class EntryHandler;
    friend class LoginHandler;
    friend class LogoutHandler;
    friend class RegisterHandler;

    friend class UserHandler;
    
    friend class FriendExitHandler;

    friend class GroupHandler;
    friend class GroupSendHandler;
    friend class GroupExitHandler;

    friend class SearchUserHandler;
    friend class SearchUserExistHandler;
    friend class FriendAddHandler;
    friend class GetFriendListHandler;
    friend class FriendSendHandler;

    friend class SearchAIRobotHandler;
    friend class SearchAIRobotExistHandler;
    friend class AIRobotAddHandler;
    friend class GetAIRobotListHandler;
    friend class AIRobotSendHandler;

    friend class OfflineMessageShowHandler;
    friend class OfflineAIMessageShowHandler;
    
    http::HttpServer httpServer_;
    http::MysqlUtil mysqlUtil_;

    // std::unordered_map<int, bool> onlineUsers_;
    std::unordered_map<int, int> onlineUsers_;  // 多种状态
    std::mutex mutexForOnlineUsers_;

    // 多个用户和多个机器人对应
    // std::unordered_map<int, std::shared_ptr<AIHelper>> chatInformation;
    //                userid                 robotid
    std::unordered_map<int, std::unordered_map<int, std::shared_ptr<AIHelper>>> chatInformation;

    std::mutex mutexForChatInformation;

};

#endif