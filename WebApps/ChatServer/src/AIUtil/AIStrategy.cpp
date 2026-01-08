#include "../include/AIUtil/AIStrategy.h"
#include "../include/AIUtil/AIFactory.h"
#include <chrono>
#include <iomanip>  // 需要包含这个头文件

// =====================================================================================================
// ========================================== AliyunStrategy ===========================================
// =====================================================================================================

AliyunStrategy::AliyunStrategy()
{
    // 在系统的环境变量中查找名为 "DASHSCOPE_API_KEY" 的变量
    // const char* key = std::getenv("DASHSCOPE_API_KEY");
    // if(!key) throw std::runtime_error("Aliyun API Key not found!");
    // apiKey_ = key;
    apiKey_ = "";
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

void AliyunStrategy::processRequest(json& payload, json& msgArray)
{
    payload["messages"] = msgArray;
}

std::pair<std::string, int64_t> AliyunStrategy::processResponse(const json& response)
{
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

// =====================================================================================================
// ======================================= DouBaoStrategy ==============================================
// =====================================================================================================

DouBaoStrategy::DouBaoStrategy()
{
    // const char* key = std::getenv("DOUBAO_API_KEY");
    // if(!key) throw std::runtime_error("DOUBAO API Key not found!");
    // apiKey_ = key;
    apiKey_ = "";
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
    return "doubao-seed-1-6-thinking-250715";
}

void DouBaoStrategy::processRequest(json& payload, json& msgArray)
{
    payload["messages"] = msgArray;
}

std::pair<std::string, int64_t> DouBaoStrategy::processResponse(const json& response)
{
    if(response.contains("choices") && !response["choices"].empty())
    {
        std::string answer = response["choices"][0]["message"]["content"];
        // 保存AI回复
        // addMessage(userId, userName, false, answer);
        int64_t timestamp = response["created"];
        return {answer, timestamp};
    }

    return {"[Error] 无法解析响应", -1};
}

// =====================================================================================================
// ========================================= AliyunRAGStrategy =========================================
// =====================================================================================================

AliyunRAGStrategy::AliyunRAGStrategy()
{
    // const char* key = std::getenv("DASHSCOPE_API_KEY");
    // if(!key) throw std::runtime_error("Aliyun API Key not found!");
    // apiKey_ = key;
    apiKey_ = "";
    applicationId_ = "";
    isMCPModel = false;
}

std::string AliyunRAGStrategy::getApiUrl() const
{
    // const char* key = std::getenv("Knowledge_Base_ID");
    // if(!key) throw std::runtime_error("knowledge_Base_ID not found!");
    // std::string id(key);
    // std::string id = "9887ab9fd0244428a1ce8d850e0409be";
    return "https://dashscope.aliyuncs.com/api/v1/apps/" + applicationId_ + "/completion";
}

std::string AliyunRAGStrategy::getApiKey() const
{
    return apiKey_;
}

std::string AliyunRAGStrategy::getModel() const
{
    return "";
}

// AliyunRAG的输入输出json和上面两个有所不同
void AliyunRAGStrategy::processRequest(json& payload, json& msgArray)
{
    // 所有的消息包括问题都被组织在msgArray中
    payload["input"]["messages"] = msgArray;
    payload["parameters"] = json::object();
    payload["debug"] = json::object();
}

std::pair<std::string, int64_t> AliyunRAGStrategy::processResponse(const json& response)
{
    if(response.contains("output") && response["output"].contains("text"))
    {
        std::string answer = response["output"]["text"];
        auto now = std::chrono::system_clock::now();
        int64_t timestamp = std::chrono::duration_cast<std::chrono::seconds>(now.time_since_epoch()).count();
        return {answer, timestamp};
    }

    return {"[Error] 无法解析响应", -1};
}

// =====================================================================================================
// ========================================= AliyunMcpStrategy =========================================
// =====================================================================================================

AliyunMcpStrategy::AliyunMcpStrategy()
{
    // const char* key = std::getenv("DASHSCOPE_API_KEY");
    // if(!key) throw std::runtime_error("Aliyun API Key not found!");
    // apiKey_ = key;
    isMCPModel = true;
}

std::string AliyunMcpStrategy::getApiUrl() const
{
    return "https://dashscope.aliyuncs.com/compatible-mode/v1/chat/completions";
}

std::string AliyunMcpStrategy::getApiKey() const
{
    return apiKey_;
}

std::string AliyunMcpStrategy::getModel() const
{
    return "qwen-plus";
}

void AliyunMcpStrategy::processRequest(json& payload, json& msgArray)
{
    payload["input"]["prompt"] = msgArray;
    payload["parameters"] = json::object();
    payload["debug"] = json::object();
}

std::pair<std::string, int64_t> AliyunMcpStrategy::processResponse(const json& response)
{
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

/*
    在AIStrategy.cpp中，提供一个注册的类，
    template<typename T>
    struct StrategyRegister{
        StrategyRegister(const std::string& name){
            StrategyFactory::instance().registerStrategy(name, [](){
                std::shared_ptr<AIStrategy> instance = std::make_shared<T>();
                return instance;
            });
        }
    };

    StrategyFactory::instance().registerStrategy()
    这个方法会把策略注册到AIFactory中的容器当中
    std::unordered_map<std::string, Creator> creators;
    因此下面四行语句就包含了注册过程。
*/

// 经过调试，这四行代码确实能在程序刚运行时就调用
static StrategyRegister<AliyunStrategy> regAliyun("1");
static StrategyRegister<DouBaoStrategy> reDoubao("2");
static StrategyRegister<AliyunRAGStrategy> regAliyunRag("3");
static StrategyRegister<AliyunMcpStrategy> regAliyunMcp("4");

