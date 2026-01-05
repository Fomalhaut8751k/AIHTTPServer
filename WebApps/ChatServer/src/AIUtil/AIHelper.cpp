#include "../../include/AIUtil/AIHelper.h"
#include "../../include/AIUtil/MQManager.h"

#include <stdexcept>
#include <chrono>

// 默认构造函数
AIHelper::AIHelper()
{
    // 默认使用阿里云大模型
    strategy = StrategyFactory::instance().create("1");  
    /*  虽然叫做create但实际上没有创建，正常查找map，没找到就抛出异常

        static StrategyRegister<AliyunStrategy> regAliyun("1");
        static StrategyRegister<DouBaoStrategy> reDoubao("2");
        static StrategyRegister<AliyunRAGStrategy> regAliyunRag("3");
        static StrategyRegister<AliyunMcpStrategy> regAliyunMcp("4");
    */
}

// 有参构造函数
AIHelper::AIHelper(const int& strategyType, const std::string& apiKey)
{
    // 下面的strategy是AIHelper成员变量
    strategy = StrategyFactory::instance().create(std::to_string(strategyType));
    strategy->setApiKey(apiKey);
}

// 有参构造函数(第一版，后面不用)
AIHelper::AIHelper(const std::string& apiKey):
    apiKey_(apiKey)
{   
    
}

// 设置默认类型
void AIHelper::setModel(const std::string& modelName)
{
    model_ = modelName;
}

// 设置默认策略(AI模型)
void AIHelper::setStrategy(std::shared_ptr<AIStrategy> strat)
{
    strategy = strat;
}

// 从策略中获取模型的API
std::string AIHelper::getApiKeyFromStrategy() const
{
    return strategy->getApiKey();
}

// 添加一条用户消息
void AIHelper::addMessage(const std::string& userInput, int64_t timestamp)
{
    std::string msStr = timestampToString(timestamp);
    messages.push_back({userInput, msStr});
    // 消息队列异步入库
    // pushMessageToMysql(userId, userName, is_user, userInput, ms);
}

// void AIHelper::restoreMessage(const std::string& userInput, long long ms)
void AIHelper::restoreMessage(const std::string& userInput, const std::string& ms)
{
    messages.push_back({userInput, ms});
}

// 发送聊天消息
// std::string AIHelper::chat(int userId, std::string userName)
std::pair<std::string, int64_t> AIHelper::chat(int userId)
{
    // 构造 payload
    json payload;
    payload["model"] = model_;
    json msgArray = json::array();

    for(size_t i = 0; i < messages.size(); ++i)
    {
        json msg;
        if(i % 2 == 0)
        {   // 偶数下标：用户
            msg["role"] = "user";
            msg["content"] = messages[i].first;
        }
        else
        {   // 奇数下标：AI
            msg["role"] = "assistant";
            msg["content"] = messages[i].first;
        }
        msgArray.push_back(msg);
    }
    payload["messages"] = msgArray;

    // 打印 payload (缩进4个空格)
    std::cout << "[DEBUG] payload = " << payload.dump(4) << std::endl;

    // 执行请求
    json response = executeCurl(payload);

    std::cerr << response << std::endl;

    if(response.contains("choices") && !response["choices"].empty())
    {
        std::string answer = response["choices"][0]["message"]["content"];
        int64_t timestamp = response["created"];
        // 保存AI回复
        // addMessage(userId, userName, false, answer);
        return {answer, timestamp};
    }

    return {"[Error] 无法解析响应", -1};
}

// 发送自定义的请求体
json AIHelper::request(const json& payload)
{
    return executeCurl(payload);
}

// std::vector<std::pair<std::string, long long>> AIHelper::GetMessage()
std::vector<std::pair<std::string, std::string>> AIHelper::GetMessage()
{
    return this->messages;
}

// 内部方法：执行curl请求
json AIHelper::executeCurl(const json& payload)
{
    CURL* curl = curl_easy_init();
    if(!curl)
    {
        throw std::runtime_error("Failed to initialize curl");
    }
    std::string readBuffer;
    struct curl_slist* headers = nullptr;
    std::string authHeader = "Authorization: Bearer " + apiKey_;

    headers = curl_slist_append(headers, authHeader.c_str());
    headers = curl_slist_append(headers, "Content-Type: application/json");

    std::string payloadStr = payload.dump();
    std::cout << "test json->payloadstr" << payloadStr << std::endl;
    curl_easy_setopt(curl, CURLOPT_URL, apiUrl_.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, payloadStr.c_str());
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, WriteCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);

    CURLcode res = curl_easy_perform(curl);
    curl_slist_free_all(headers);
    curl_easy_cleanup(curl);

    if(res != CURLE_OK)
    {
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(res)));
    }

    try
    {
        return json::parse(readBuffer);
    }
    catch(...)
    {
        throw std::runtime_error("Failed to parse JSON response: " + readBuffer);
    }
    
}

// curl 回调函数，把返回的数据写到string Buffer
size_t AIHelper::WriteCallback(void* contents, size_t size, size_t nmemb, void* userp)
{
    size_t totalSize = size * nmemb;
    std::string* buffer = static_cast<std::string*>(userp);
    buffer->append(static_cast<char*>(contents), totalSize);
    return totalSize;
}

std::string AIHelper::escapeString(const std::string& input)
{
    std::string output;
    output.reserve(input.size() * 2);
    for(char c: input)
    {
        switch(c)
        {
            case '\\': output += "\\\\"; break;
            case '\'': output += "\\\'"; break;
            case '\"': output += "\\\""; break;
            case '\n': output += "\\n"; break;
            case '\r': output += "\\r"; break;
            case '\t': output += "\\t"; break;
            default: output += c; break;
        }
    }
    return output;
}

void AIHelper::pushMessageToMysql(int userId,
                                  const std::string& userName,
                                  bool is_user,
                                  const std::string& userInput,
                                  long long ms)
{
    std::string safeUserName = escapeString(userName);
    std::string safeUserInput = escapeString(userInput);

    std::string sql = "INSERT INTO chat_message (id, username, is_user, content, ts) VALUES ("
        + std::to_string(userId) + ", "
        + "'" + safeUserName + "', "
        + std::to_string(is_user ? 1 : 0) + ", "
        + "'" + safeUserInput + "', "
        + std::to_string(ms) + ")";

    // 改成消息队列异步执行mysql操作，用于流量削峰和解耦逻辑
    MQManager::instance().publish("sql_queue", sql);
}

void AIHelper::pushMessageToMysql(const int& userId, const int& robotId,
                            const std::string& message, const std::string& source, int64_t timestamp)
{
    std::string safeMessage = escapeString(message);
    // std::string sql = "INSERT INTO offlineAIRobotMessage (userid, robotid, message, source, created_at) VALUES (?, ?, ?, ?, FROM_UNIXTIME(?))";
    std::string sql = "INSERT INTO offlineAIRobotMessage (userid, robotid, message, source, created_at) VALUES ("
        + std::to_string(userId) + ", "
        + std::to_string(robotId) + ", "
        + "'" + safeMessage + "'" + ", "
        + "'" + source + "'" + ", "
        + "FROM_UNIXTIME(" + std::to_string(timestamp) + "))";
        // + timestampToString(timestamp) + ")";
    MQManager::instance().publish("sql_queue", sql);
}

std::string AIHelper::timestampToString(int64_t timestamp) 
{
    // timestamp通常是秒级，如果timestamp是毫秒级则除以1000
    time_t t = static_cast<time_t>(timestamp);
    struct tm* tm_info = localtime(&t);  // 或 gmtime() 获取UTC时间
    
    char buffer[20];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", tm_info);
    
    return std::string(buffer);
}