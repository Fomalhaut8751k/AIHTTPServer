#include "../include/AIUtil/AIFactory.h"

StrategyFactory& StrategyFactory::instance(){
    static StrategyFactory factory;
    return factory;
}

void StrategyFactory::registerStrategy(const std::string& name, Creator creator){
    creators[name] = std::move(creator);
}

std::shared_ptr<AIStrategy> StrategyFactory::create(const std::string& name){
    // for(auto item: creators){
    //     std::cerr << item.first << ": " << &(item.second) << std::endl;
    // }
    auto it = creators.find(name);
    if(it == creators.end()){
        throw std::runtime_error("Unknown strategy: " + name);
    }
    /*  it->second是一个函数: 
        using Creator = std::function<std::shared_ptr<AIStrategy>()>;
        it->second()就是执行这个函数
        lammda表达式:
        [] {
            std::shared_ptr<AIStrategy> instance = std::make_shared<T>();
            return instance;
        }
    */
    return it->second();
}