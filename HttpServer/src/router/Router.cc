#include "../../include/router/Router.h"

namespace http
{

namespace router
{

size_t Router::RouteKeyHash::operator()(const RouteKey& key) const
{
    size_t methodHash = std::hash<int>{}(static_cast<int>(key.method));
    size_t pathHash = std::hash<std::string>{}(key.path);
    return methodHash * 31 + pathHash;
}




std::regex Router::convertToRegex(const std::string &pathPattern)
{   
    std::string regexPattern = "^" + std::regex_replace(pathPattern, std::regex(R"(/:([^/]+))"), R"(/([^/]+))") + "$";
    return std::regex(regexPattern);
}

void Router::extractPathParameters(const std::smatch &match, HttpRequest& request)
{
    for(size_t i = 1; i < match.size(); ++i)
    {
        request.setPathParameters("param" + std::to_string(i), match[i].str());
    }
}

}

}
