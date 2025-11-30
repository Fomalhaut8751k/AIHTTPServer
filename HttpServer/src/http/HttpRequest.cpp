#include "../../include/http/HttpRequest.h"

#include <cassert>

namespace http
{

HttpRequest::HttpRequest():
    method_(kInvalid),
    version_("Unknown")
{

}

void HttpRequest::setReceiveTime(TimeStamp t)
{

}

/*
    const char* start 和 const char* end
    表示了字符串的起始位置和结束位置(左闭右开)
    string m(start, end)就能够获得对于的字符串
*/

bool HttpRequest::setMethod(const char* start, const char* end)
{
    assert(method_ == Method::kInvalid);
    std::string m(start, end);

    if(m == "GET") { method_ = Method::kGet; }
    else if(m == "POST") { method_ = Method::kPost; }
    else if(m == "HEAD") { method_ = Method::kHead; }
    else if(m == "PUT") { method_ = Method::kPut; }
    else if(m == "DELETE") { method_ = Method::kDelete; }
    else if(m == "OPTIONS") { method_ = Method::kOptions; }
    else { method_ = Method::kInvalid; }

    return method_ != Method::kInvalid;  // 判断是否修改成功
}


void HttpRequest::setPath(const char* start, const char* end)
{
    // path_ = std::string(start, end);  // 会创建临时对象
    path_.assign(start, end);  // 直接在path_的内存上操作
}


void HttpRequest::setPathParameters(const std::string &key, const std::string &value)
{
    pathParameters_[key] = value;
}

std::string HttpRequest::getPathParameters(const std::string &key) const
{
    auto it = pathParameters_.find(key);
    if(it != pathParameters_.end())
    {
        return it->second;
    }
    return "";
}


void HttpRequest::setQueryParameters(const char* start, const char* end)
{   // 带参数的请求行例子：page=2&limit=20&sort=name&order=asc
    std::string argumentStr(start, end);
    std::string::size_type pos = 0;
    std::string::size_type prev = 0;

    // 按照‘&’分割参数列表
    while((pos = argumentStr.find('&', prev)) != std::string::npos)
    {
        std::string pair = argumentStr.substr(prev, pos-prev);
        std::string::size_type equalPos = pair.find('=');
        if(equalPos != std::string::npos)
        {
            std::string key = pair.substr(prev, equalPos);
            std::string value = pair.substr(equalPos+1, pos);
            queryParameters_[key] = value;
        }
        prev = pos + 1;
    }
    // 因为&不会出现在最后一个参数的结尾，因此最后一个要单独处理一下
    std::string pair = argumentStr.substr(prev, pos-prev);
    std::string::size_type equalPos = pair.find('=');
    if(equalPos != std::string::npos)
    {
        std::string key = pair.substr(prev, equalPos);
        std::string value = pair.substr(equalPos+1, pos);
        queryParameters_[key] = value;
    }

}

std::string HttpRequest::getQueryParameters(const std::string &key) const
{
    auto it = queryParameters_.find(key);
    if(it != pathParameters_.end())
    {
        return it->second;
    }
    return "";
}


void HttpRequest::addHeader(const char* start, const char* colon, const char* end)
{   // 请求头里有很多组，应该多次调用
    std::string key(start, colon++);  // colon应该是“：”的位置
    while(colon < end && isspace(*colon))  // 跳过空格
    {
        ++colon;
    }
    std::string value(colon, end);
    while(!value.empty() && isspace(value[value.size() - 1]))  // 判断结尾是否是空
    {
        value.resize(value.size() - 1);
    }
    headers_[key] = value;
}

std::string HttpRequest::getHeadler(const std::string& field) const
{   // field: “Host”, "User-Agent", .....
    auto it = headers_.find(field);
    if(it != headers_.end())
    {
        return it->second;
    }
    return "";
}


void HttpRequest::setBody(const char* start, const char* end)
{   // 应该就是直接{}的一托
    content_.assign(start, end);
}


void HttpRequest::swap(HttpRequest& that)
{
    std::swap(method_, that.method_);
    std::swap(path_, that.path_);
    std::swap(pathParameters_, that.pathParameters_);
    std::swap(queryParameters_, that.queryParameters_);
    std::swap(version_, that.version_);
    std::swap(headers_, that.headers_);
    std::swap(receiveTime_, that.receiveTime_);
}


}