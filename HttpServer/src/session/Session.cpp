#include "../../include/session/Session.h"
#include "../../include/session/SessionManager.h"

namespace http
{

namespace session
{

Session::Session(const std::string& sessionId, 
            SessionManager* sessionManger, int maxAge=3600):
    sessionId_(sessionId),
    sessionManager_(sessionManger),
    maxAge_(maxAge_)
{
    refresh();
}

bool Session::isExpired() const
{
    return std::chrono::system_clock::now() > expiryTime_;
}

void Session::refresh()
{
    expiryTime_ = std::chrono::system_clock::now() + std::chrono::seconds(maxAge_);
}

// 数据存取
void Session::setValue(const std::string &key, const std::string &value)
{
    data_[key] = value;
    // 如果设置了manager，自动保存更改
    if(sessionManager_)
    {
        sessionManager_->updateSession(shared_from_this());
    }
}

std::string Session::getValue(const std::string &key) const
{
    auto it = data_.find(key);
    return it != data_.end() ? it->second: "";
}

void Session::remove(const std::string &key)
{   
    data_.erase(key);
}

void Session::clear()
{
    data_.clear();
}

}


}