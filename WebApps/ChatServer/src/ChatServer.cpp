#include "../include/ChatServer.h"

#include "../../../HttpServer/include/http/HttpRequest.h"
#include "../../../HttpServer/include/http/HttpResponse.h"
#include "../../../HttpServer/include/http/HttpServer.h"

#include "../include/handlers/EntryHandler.h"
#include "../include/handlers/FriendExitHandler.h"
#include "../include/handlers/FriendHandler.h"
#include "../include/handlers/FriendSendHandler.h"
#include "../include/handlers/GroupExitHandler.h"
#include "../include/handlers/GroupHandler.h"
#include "../include/handlers/GroupSendHandler.h"
#include "../include/handlers/LoginHandler.h"
#include "../include/handlers/LogoutHandler.h"
#include "../include/handlers/RegisterHandler.h"
#include "../include/handlers/UserHandler.h"
#include "../include/handlers/SearchHandler.h"
#include "../include/handlers/SearchUserHandler.h"
#include "../include/handlers/FriendAddHandler.h"
#include "../include/handlers/GetFriendListHandler.h"
#include "../include/handlers/OfflineMessageShowHandler.h"

using namespace http;

std::string ChatServer::registerInvitationCode = "D90AS809SJ0EW092JK23SD";

ChatServer::ChatServer(int port, const std::string& name, 
                    TcpServer::Option option):
    httpServer_(port, name, option)
{
    initialize();
}

void ChatServer::setThreadNum(int numThreads)
{
    httpServer_.setThreadNum(numThreads);
}

void ChatServer::start()
{
    httpServer_.start();
}

void ChatServer::initialize()
{
    // 初始化数据库连接池
    http::MysqlUtil::init("tcp://127.0.0.1:3306", "root", "root", "chat", 10);
    // 初始化会话
    initializeSession();
    // 初始化中间件
    initializeMiddleWare();
    // 初始化路由
    initializeRouter();
}

void ChatServer::initializeSession()
{
    // 创建会话存储
    auto sessionStorage = std::make_unique<http::session::MemorySessionStorage>();
    // 创建会话管理器
    auto sessionManager = std::make_unique<http::session::SessionManager>(std::move(sessionStorage));
    // 设置会话管理器
    setSessionManager(std::move(sessionManager));
}

void ChatServer::initializeRouter()
{
    // 登录注册入口页面
    httpServer_.Get("/", std::make_shared<EntryHandler>(this));
    httpServer_.Get("/entry", std::make_shared<EntryHandler>(this));
    // 登录
    httpServer_.Post("/login", std::make_shared<LoginHandler>(this));
    // 注册
    httpServer_.Post("/register", std::make_shared<RegisterHandler>(this));
    // 登出
    httpServer_.Post("/user/logout", std::make_shared<LoginHandler>(this));
    // 用户界面
    httpServer_.Get("/user", std::make_shared<UserHandler>(this));
    // 好友聊天
    httpServer_.Get("/user/friend", std::make_shared<FriendHandler>(this));
    // 向好友发送聊天消息
    httpServer_.Post("/user/friend/send", std::make_shared<FriendSendHandler>(this));
    // 退出好友聊天
    httpServer_.Get("/user/friend/exit", std::make_shared<FriendExitHandler>(this));
    // 群聊聊天
    httpServer_.Get("/user/group", std::make_shared<GroupHandler>(this));
    // 向群聊发送聊天消息
    httpServer_.Post("/user/group/send", std::make_shared<GroupSendHandler>(this));
    // 退出群聊聊天
    httpServer_.Get("/user/group/exit", std::make_shared<GroupExitHandler>(this));
    // 陌生人查找
    httpServer_.Post("/user/search", std::make_shared<SearchHandler>(this));
    // 查询陌生人是否存在
    httpServer_.Post("/user/search/exist", std::make_shared<SearchUserHandler>(this));
    // 确认添加好友
    httpServer_.Post("/user/search/add", std::make_shared<FriendAddHandler>(this));
    // 返回好友列表
    httpServer_.Post("/user/friendlist", std::make_shared<GetFriendListHandler>(this));
    // 加载离线消息
    httpServer_.Post("/user/offlinemessage", std::make_shared<OfflineMessageShowHandler>(this));
}

void ChatServer::initializeMiddleWare()
{
    // 创建中间件
    auto corsMiddleware = std::make_shared<http::middleware::CorsMiddleware>();
    // 添加中间件
    httpServer_.addMiddleware(corsMiddleware);
}

void ChatServer::setSessionManager(std::unique_ptr<http::session::SessionManager> manager)
{
    httpServer_.setSeesionManager(std::move(manager));
}

http::session::SessionManager* ChatServer::getSessionManager() const
{
    return httpServer_.getSessionManager();
}

void ChatServer::packageResp(const std::string& version, http::HttpResponse::HttpStatusCode statusCode, 
                    const std::string& statusMsg, bool close, const std::string& contentType,
                    int contentLen, const std::string& body, http::HttpResponse* resp)
{
    if(!resp)
    {
        logger_->ERROR("Response pointer is null");
        return;
    }

    try
    {
        resp->setVersion(version);
        resp->setStatusCode(statusCode);
        resp->setStatusMessage(statusMsg);
        resp->setCloseConnection(close);
        resp->setContentType(contentType);
        resp->setContentLength(contentLen);
        resp->setBody(body);

        logger_->INFO("Response packaged successfully");
    }
    catch(const std::exception& e)
    {
        logger_->ERROR(std::string("Error in packageResp: ") + e.what());
        // 设置一个基本的错误响应
        resp->setStatusCode(http::HttpResponse::k500InternalServerError);
        resp->setStatusMessage("Internal Server Error");
        resp->setCloseConnection(true);
    }
}