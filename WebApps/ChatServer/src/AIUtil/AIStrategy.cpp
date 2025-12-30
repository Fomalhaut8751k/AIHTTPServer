#include "../include/AIUtil/AIStrategy.h"
#include "../include/AIUtil/AIFactory.h"

// =========== AliyunStrategy ===========
AliyunStrategy::AliyunStrategy()
{
    const char* key = std::getenv("DASHSCOPE_API_KEY");
    if(!key) throw std::runtime_error("Aliyun API Key not found!");
    apiKey_ = key;
    isMCPModel = true;
}

std::string AliyunStrategy::getApiUrl() const
{
    return "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions";
}

std::string AliyunStrategy::getApiKey() const
{
    return apiKey_;
}

std::string AliyunStrategy::getModel() const
{
    return "qwen-plus";
}

json AliyunStrategy::buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const
{
    return json();
}

std::string AliyunStrategy::parseResponse(const json& response) const
{
    return "";
}

// =========== DouBaoStrategy ===========
DouBaoStrategy::DouBaoStrategy()
{
    const char* key = std::getenv("DOUBAO_API_KEY");
    if(!key) throw std::runtime_error("DOUBAO API Key not found!");
    apiKey_ = key;
    isMCPModel = false;
}

std::string DouBaoStrategy::getApiUrl() const
{
    return "https://ark.cn-beijing.volces.com/api/v3/chat/completions";
}

std::string DouBaoStrategy::getApiKey() const
{
    return apiKey_;
}

std::string DouBaoStrategy::getModel() const
{
    return "doubao-sees-1-6-thinking-250715";
}

json DouBaoStrategy::buildRequest(const std::vector<std::pair<std::string, long long>>& message) const
{
    return json();
}

std::string DouBaoStrategy::parseResponse(const json& response) const
{
    return "";
}


// =========== AliyunRAGStrategy =========== 
AliyunRAGStrategy::AliyunRAGStrategy()
{

}

std::string AliyunRAGStrategy::getApiUrl() const
{
    return "";
}

std::string AliyunRAGStrategy::getApiKey() const
{
    return "";
}

std::string AliyunRAGStrategy::getModel() const
{
    return "";
}

json AliyunRAGStrategy::buildRequest(const std::vector<std::pair<std::string, long long>>& message) const
{
    return json();
}

std::string AliyunRAGStrategy::parseResponse(const json& response) const
{
    return "";
}


// =========== AliyunMcpStrategy =========== 
AliyunMcpStrategy::AliyunMcpStrategy()
{
    
}

std::string AliyunMcpStrategy::getApiUrl() const
{
    return "";
}

std::string AliyunMcpStrategy::getApiKey() const
{
    return "";
}

std::string AliyunMcpStrategy::getModel() const
{
    return "";
}

json AliyunMcpStrategy::buildRequest(const std::vector<std::pair<std::string, long long>>& message) const
{
    return json();
}

std::string AliyunMcpStrategy::parseResponse(const json& response) const
{
    return "";
}


