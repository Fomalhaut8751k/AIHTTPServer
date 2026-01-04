#include "../include/AIUtil/AIStrategy.h"
#include "../include/AIUtil/AIFactory.h"

// =========== AliyunStrategy ===========
AliyunStrategy::AliyunStrategy()
{
    // 在系统的环境变量中查找名为 "DASHSCOPE_API_KEY" 的变量
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


// =========== AliyunRAGStrategy =========== 
AliyunRAGStrategy::AliyunRAGStrategy()
{
    const char* key = std::getenv("DASHSCOPE_API_KEY");
    if(!key) throw std::runtime_error("Aliyun API Key not found!");
    apiKey_ = key;
    isMCPModel = false;
}

std::string AliyunRAGStrategy::getApiUrl() const
{
    const char* key = std::getenv("Knowledge_Base_ID");
    if(!key) throw std::runtime_error("knowledge_Base_ID not found!");
    std::string id(key);
    return "https://dashscope.aliyuncs.com/api/v1/apps/" + id + "/completion";
}

std::string AliyunRAGStrategy::getApiKey() const
{
    return apiKey_;
}

std::string AliyunRAGStrategy::getModel() const
{
    return "";
}


// =========== AliyunMcpStrategy =========== 
AliyunMcpStrategy::AliyunMcpStrategy()
{
    const char* key = std::getenv("DASHSCOPE_API_KEY");
    if(!key) throw std::runtime_error("Aliyun API Key not found!");
    apiKey_ = key;
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

static StrategyRegister<AliyunStrategy> regAliyun("1");
static StrategyRegister<DouBaoStrategy> reDoubao("2");
static StrategyRegister<AliyunRAGStrategy> regAliyunRag("3");
static StrategyRegister<AliyunMcpStrategy> regAliyunMcp("4");

