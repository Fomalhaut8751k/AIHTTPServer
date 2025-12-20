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

class EntryHandler;
class LoginHandler;
class LogoutHandler;
class RegisterHandler;
class UserHandler;
class FriendHandler;
class FriendSendHandler;
class FriendExitHandler;
class GroupHandler;
class GroupSendHandler;
class GroupExitHandler;
class SearchUserHandler;

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
    friend class FriendHandler;
    friend class FriendSendHandler;
    friend class FriendExitHandler;
    friend class GroupHandler;
    friend class GroupSendHandler;
    friend class GroupExitHandler;
    friend class SearchUserHandler;

    http::HttpServer httpServer_;
    http::MysqlUtil mysqlUtil_;

    std::unordered_map<int, bool> onlineUsers_;
    std::mutex mutexForOnlineUsers_;
};

#endif