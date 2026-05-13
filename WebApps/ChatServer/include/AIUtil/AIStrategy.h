#ifndef AISTRATEGY_H
#define AISTRATEGY_H

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <memory>

#include "../../../../HttpServer/include/utils/JsonUtil.h"


class AIStrategy
{
public:
    virtual ~AIStrategy() = default;

    /*
        ApiUrl 和 Model 是固定的，比如
        return "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions";
        return "qwen-plus";

        而 ApiKey是要手动设定的，因此提供另外提供
    */
    virtual void setApiKey(const std::string api_key) = 0;
    virtual std::string getStrategyType() const = 0;

    virtual void processRequest(json& payload, json& msgArray) = 0;
    virtual std::pair<std::string, int64_t> processResponse(const json& response) = 0;

    virtual std::string getApiUrl() const = 0;  // 模型的网站
    virtual std::string getApiKey() const = 0;  // API Key
    virtual std::string getModel() const = 0;  // 模型

    /*
        将从数据库读取到的消息按照用户-AI-用户-AI这样一问一答的顺序记录在一个json串中返回
        该功能已在其他地方实现，故这里不再实现
    */
    // virtual json buildRequest(const std::vector<std::pair<std::string, long long>>& message) const = 0;

    /* 
        从AI返回的json串中(除了答案还有时间戳等杂七杂八的东西)中获取消息的内容
        该功能已在其他地方实现，故这里不再实现
    */
    // virtual std::string parseResponse(const json& response) const = 0;

    bool isMCPModel = false;
};

class AliyunStrategy: public AIStrategy
{
public: 
    AliyunStrategy();

    void setApiKey(const std::string api_key) override { apiKey_ = api_key; }
    std::string getStrategyType() const override { return "1"; }
    
    void processRequest(json& payload, json& msgArray) override;  // 预处理发送json
    std::pair<std::string, int64_t> processResponse(const json& response) override;  // 提取接受json

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;
private:
    std::string apiKey_;
};

class DouBaoStrategy: public AIStrategy
{
public:
    DouBaoStrategy();

    void setApiKey(const std::string api_key) override { apiKey_ = api_key; }
    std::string getStrategyType() const override { return "2"; }

    void processRequest(json& payload, json& msgArray) override;
    std::pair<std::string, int64_t> processResponse(const json& response) override;

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;
private:
    std::string apiKey_;
};

class AliyunRAGStrategy: public AIStrategy
{
public:
    AliyunRAGStrategy();

    void setApiKey(const std::string api_key) override { apiKey_ = api_key; }
    std::string getStrategyType() const override { return "3"; }

    void processRequest(json& payload, json& msgArray) override;
    std::pair<std::string, int64_t> processResponse(const json& response) override;

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    void setApplicationId(const std::string application_id) { applicationId_ = application_id; }
    std::string getApplicationId() const { return applicationId_; }

private:
    std::string apiKey_;
    std::string applicationId_; // RAG特有的智能体应用ID
};

class AliyunMcpStrategy: public AIStrategy
{
public:
    AliyunMcpStrategy();

    void setApiKey(const std::string api_key) override { apiKey_ = api_key; }
    std::string getStrategyType() const override { return "4"; }

    void processRequest(json& payload, json& msgArray) override;
    std::pair<std::string, int64_t> processResponse(const json& response) override;

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;
private:
    std::string apiKey_;
};

class ClaudeOpusStrategy: public AIStrategy
{
public:
    ClaudeOpusStrategy();

    void setApiKey(const std::string api_key) override { apiKey_ = api_key; }
    std::string getStrategyType() const override { return "5"; }

    void processRequest(json& payload, json& msgArray) override;
    std::pair<std::string, int64_t> processResponse(const json& response) override;

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

private:
    std::string apiKey_;
};

class GptStrategy: public AIStrategy
{
public:
    GptStrategy();

    void setApiKey(const std::string api_key) override { apiKey_ = api_key; }
    std::string getStrategyType() const override { return "6"; }

    void processRequest(json& payload, json& msgArray) override;
    std::pair<std::string, int64_t> processResponse(const json& response) override;

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

private:
    std::string apiKey_;
};

#endif
