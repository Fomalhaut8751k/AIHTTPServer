#ifndef AIFACTORY_H
#define AIFACTORY_H

#include <string>
#include <vector>
#include <utility>
#include <iostream>
#include <sstream>
#include <memory>
#include <functional>
#include <unordered_map>
#include <string>

#include "AIStrategy.h"

class StrategyFactory
{
public:
    using Creator = std::function<std::shared_ptr<AIStrategy>()>;

    // 还是一个单例模式的工厂
    static StrategyFactory& instance();

    void registerStrategy(const std::string& name, Creator creator);

    // 把AIStragedy的创建封装在工厂中，
    std::shared_ptr<AIStrategy> create(const std::string& name);

private:
    StrategyFactory() = default;
    std::unordered_map<std::string, Creator> creators;  // 记录所有注册后的策略
};

template<typename T>
struct StrategyRegister{
    StrategyRegister(const std::string& name){
        StrategyFactory::instance().registerStrategy(name, [](){
            std::shared_ptr<AIStrategy> instance = std::make_shared<T>();
            return instance;
        });
    }
};

#endif