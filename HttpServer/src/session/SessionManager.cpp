#include "../../include/session/SessionManager.h"

#include <iomanip>
#include <iostream>
#include <sstream>
#include <cassert>

namespace http
{

namespace session
{

// 初始化会话管理器，设置会话存储对象和随机数生成器
SessionManager::SessionManager(std::unique_ptr<SessionStorage> storage):
    storage_(std::move(storage)),
    rng_(std::random_device{}())  // 初始化随机数生成器，用于生成一个随机的会话ID
{

}

// 从请求中获取或创建会话
std::shared_ptr<Session> SessionManager::getSession(const HttpRequest& req, HttpResponse* resp)
{
    assert(storage_ != nullptr);
    auto it = storage_->load(); 
    if(it)
    {
        return it;
    }
    else
    {
        storage_->save();
    }
}

// 销毁会话
void SessionManager::destorySession(const std::string& sessionId)
{

}

// 清理过期会话
void SessionManager::cleanExpiredSession()
{

}

// 更新会话
void SessionManager::updateSession(std::shared_ptr<Session> session)
{

}

// 生产唯一的会话标识符，确保会话的唯一性和安全性
std::string SessionManager::generateSessionId()
{
    std::stringstream ss;
    std::uniform_int_distribution<> dist(0, 15);  // dist生成的随机整数范围为0-15

    // 生成32个字符的会话ID，每个字符第一个十六进制数字
    for(int i = 0; i < 32; ++i)
    {
        ss << std::hex << dist(rng_);  // dist(rng_)生成一个随机数，转成16进制，一共32个并拼接在一起
    }
    return ss.str();
}

std::string SessionManager::getSessionIdFromCookie(const HttpRequest& req)
{

}

void SessionManager::setSessionCookie(const std::string& sessionId, HttpResponse* resp)
{

}

}
