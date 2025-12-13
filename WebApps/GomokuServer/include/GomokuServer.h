#ifndef GOMOKUSERVER_H
#define GOMOKUSERVER_H

#include <atomic>
#include <memory>
#include <tuple>
#include <unordered_map>
#include <mutex>

#include "AiGame.h"
#include "../../../HttpServer/include/http/HttpServer.h"
#include "../../../HttpServer/include/utils/MysqlUtil.h"
#include "../../../HttpServer/include/utils/FileUtil.h"
#include "../../../HttpServer/include/utils/JsonUtil.h"

class LoginHandler;
class EntryHandler;
class RegisterHandler;
class MenuHandler;
class AiGameStartHandler;
class LogoutHandler;
class AiGameMoveHandler;
class GameBackendHandler;

#define DURING_GAME 1
#define GAME_OVER 2

#define MAX_AIBOT_NUM 4096

class GomokuServer
{
public:
    GomokuServer(int port, const std::string& name,
                 TcpServer::Option option = TcpServer::kNoReusePort);
    
    void setThreadNum(int numThreads);
    void start();
private:
    void initialize();
    void initializeSession();
    void initializeRouter();
    void initializeMiddleWare();

    void setSessionManager(std::unique_ptr<http::session::SessionManager> manager);

    http::session::SessionManager* getSessionManager() const;

    void restartChessGameVsAi(const http::HttpRequest& req, http::HttpResponse* resp);
    void getBackendData(const http::HttpRequest& req, http::HttpResponse* resp);

    void packageResp(const std::string& version, http::HttpResponse::HttpStatusCode statusCode, 
                     const std::string& statusMsg, bool close, const std::string& contentType,
                     int contentLen, const std::string& body, http::HttpResponse* resp);

    // 获取历史最高在线人数
    int getMaxOnline() const;

    // 获取当前在线人数
    int getCurOnline() const;

    void updateMaxOnline(int online);

    // 获取用户总数
    int getUserCount();

    friend class LoginHandler;
    friend class EntryHandler;
    friend class RegisterHandler;
    friend class MenuHandler;
    friend class AiGameStartHandler;
    friend class LogoutHandler;
    friend class AiGameMoveHandler;
    friend class GameBackendHandler;

    enum GameType
    {
        NO_GAME = 0,
        MAN_VS_AI = 1,
        MAN_VS_MAN = 2
    };

    // 实际业务制定由GomokuServer来完成
    // 需要留意httpServer_提供哪些接口供使用
    http::HttpServer httpServer_;
    http::MysqlUtil mysqlUtil_;
    // userId->AiBot
    std::unordered_map<int, std::shared_ptr<AiGame>> aiGames_;
    std::mutex mutexForAiGames_;
    // userId->是否在游戏中
    std::unordered_map<int, bool> onlineUsers_;
    std::mutex mutexForOnlineUsers_;
    // 最高在线人数
    std::atomic<int> maxOnline_;
};

#endif