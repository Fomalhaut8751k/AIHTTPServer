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

    virtual std::string getApiUrl() const = 0;

    // API Key
    virtual std::string getApiKey() const = 0;

    virtual std::string getModel() const = 0;

    virtual json buildRequest(const std::vector<std::pair<std::string, long long>>& message) const = 0;

    virtual std::string parseResponse(const json& response) const = 0;

    bool isMCPModel = false;
};


class AliyunStrategy: public AIStrategy
{
public: 
    AliyunStrategy();

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

class DouBaoStrategy: public AIStrategy
{
    DouBaoStrategy();

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

class AliyunRAGStrategy: public AIStrategy
{
public:
    AliyunRAGStrategy();

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

class AliyunMcpStrategy: public AIStrategy
{
public:
    AliyunMcpStrategy();

    std::string getApiUrl() const override;
    std::string getApiKey() const override;
    std::string getModel() const override;

    json buildRequest(const std::vector<std::pair<std::string, long long>>& messages) const override;
    std::string parseResponse(const json& response) const override;

private:
    std::string apiKey_;
};

#endif