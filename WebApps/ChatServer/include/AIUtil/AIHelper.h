#ifndef AIHELPER_H
#define AIHELPER_H

#include <string>
#include <vector>
#include <utility>
#include <curl/curl.h>
#include <iostream>
#include <sstream>

#include "../../../../HttpServer/include/utils/JsonUtil.h"
#include "../../../../HttpServer/include/utils/MysqlUtil.h"

#include "AIFactory.h"
#include "AIConfig.h"
#include "AIToolRegistry.h"


// 封装curl访问阿里的模型
class AIHelper
{
public:
    // 默认构造函数。默认策略类型"1"
    AIHelper();
    // 有参构造函数
    AIHelper(const int& strategyType, const std::string& apikey);

    // 设置默认策略
    void setStrategy(std::shared_ptr<AIStrategy> strat);

    // 从策略中获取模型的API
    std::string getApiKeyFromStrategy() const;

    // 从策略中获取模型的URL
    std::string getApiUrlFromStrategy() const;

    // 从策略中获取模型的类型
    std::string getModelFromStrategy() const;

    // 添加一条消息
    void addMessage(const std::string& userInput, int64_t timestamp);

    // 恢复一条消息
    // void restoreMessage(const std::string& userInput, long long ms);
    void restoreMessage(const std::string& userInput, const std::string& ts);

    // 发送聊天消息，返回AI的响应内容
    // messages: [{"role": "system", "content": "..."}, {"role": "user", "content": "..."}]
    // std::string chat(int userId, std::string userName);

    // std::string chat(int userId);
    std::pair<std::string, int64_t> chat(int userId);

    // 可选：发送自定义的请求体
    json request(const json& payload);

    // std::vector<std::pair<std::string, long long>> GetMessage();
    std::vector<std::pair<std::string, std::string>> GetMessage();

    void pushMessageToMysql(int userId, const std::string& userName, bool is_user, 
                            const std::string& userInput, long long ms);

    void pushMessageToMysql(const int& userId, const int& robotId,
                            const std::string& message, const std::string& source, int64_t timestamp);

private:
    std::string escapeString(const std::string& input);
    // 加入到mysql的接口（提供加入到线程池的接口，线程池做异步mysql更新操作）
    // todo:

    // 内部方法：执行curl请求，返回原始JSON
    json executeCurl(const json& payload);

    // curl 回调函数，把返回的数据写到 string buffer
    static size_t WriteCallback(void* contents, size_t size, size_t nmemb, void* userp);

    // 时间戳类型的转化，因为
    std::string timestampToString(int64_t timestamp);

// ========================================================================================
    std::shared_ptr<AIStrategy> strategy;

    // 只存放消息内容和时间戳，因为是按顺序存放的，因此第0,2,4...即偶数个都是用户的，奇数都是机器人的
    // std::vector<std::pair<std::string, long long>> messages; 
    std::vector<std::pair<std::string, std::string>> messages;  // 时间戳换成字符串
};

#endif