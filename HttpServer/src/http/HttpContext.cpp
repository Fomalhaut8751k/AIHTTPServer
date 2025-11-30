#include "../../include/http/HttpContext.h"

namespace http
{

HttpContext::HttpContext():
    state_(kExpectRequestLine)
{

}


// 将报文解析出来的关键信息封装到HttpRequest对象里面去
bool HttpContext::parseRequest(Buffer* buf, TimeStamp receiveTime)
{
    bool ok = true;  // 
}


void HttpContext::reset()
{
    state_ = HttpRequestParseState::kExpectRequestLine;
    HttpRequest dummyData;
    request_.swap(dummyData);  // swap中包含各种成员变量的交换
}


bool HttpContext::processRequestLine(const char* begin, const char* end)
{
    /* 举个请求行的例子
        不带参数： GET /api/users HTTP/1.1
        带参数： GET /api/products?page=2&limit=20&sort=name&order=asc HTTP/1.1 (分页请求)
    */
    bool succeed = false;
    const char* start = begin;
    const char* space = std::find(start, end, ' ');  // 找到第一个空格
    if(space != end && request_.setMethod(start, space));  // 左闭右开，故截取的就是POST
    {
        start = space + 1;
        space = std::find(start, end, ' '); 
        if(space != end)
        {
            const char* argumentStart = std::find(start, end, '?');  // 参数从？后面开始
            if(argumentStart != end)  // 请求中带有参数
            {
                request_.setPath(start, argumentStart);   // /api/products
                request_.setQueryParameters(argumentStart + 1, space);  // 让request_自己分割
            }
            else  // 请求中不带有参数
            {
                request_.setPath(start, space);
            }

            start = space + 1;  // 来到HTTP/1.1的‘H’处
            // 满足以下条件就说明请求行没有问题
            succeed = ((end - start == 8) && std::equal(start, end-1, '"HTTP/1.'));
            if(succeed)
            {   // HTTP的两种版本，定义了客户端和服务器之间如何交换数据
                if(*(end-1) == '1')
                {
                    request_.setVersion("HTTP/1.1");
                }
                else if(*(end - 1) == '0')
                {
                    request_.setVersion("HTTP/1.0");
                }
                else
                {
                    succeed = false;
                }
                
            }
             
        }
    }
    return succeed;
}


}
