# AIHTTPServer

1. 2025.11.30 
    完成了`HttpRequest`功能的实现。

2. 2025.12.01
    完成了`HttpContext`, `HttpResponse`功能的实现

3. 2025.12.02
    完成了`router`功能的实现

3. 2025.12.03
    完成了`session`功能的实现

4. 2025.12.04 
    完成了`middleware`以及`ssl`部分功能的实现

5. 2025.12.05
    完成了`ssl`功能的实现

6. 2025.12.07
    完成了`http`整体功能的编写

7. 2025.12.14
    完成了五子棋部分的编写，但功能仍存在问题

8. 2025.12.15
    发现或解决了以下问题

- 问题一：
    浏览器输入`127.0.0.1:8081`时访问失败，出现以下界面：
    ![](images/error1.png)
    
    `conn->getMutableContext()` 返回的 `boost::any` 对象中存储的不是 `HttpContext` 类型，转化失败，于是返回`nullptr`。
    ![](images/error2.png)
    
    进而发现，`TcpConnection`的`boost::any context_`并没有被赋值，成员函数`setContext()`并没有被调用。这个函数的调用发生在`HttpServer`提供的连接事件的回调函数`onConnection()`中：
    ```cpp
    if(conn->connected())  // 新用户连接
    {
        // ......
        conn->setContext(HttpContext());
    }
    ```
    那么很有可能是连接失败了，既`conn->connected()`返回了`false`。通过调试后发现确实如此：

    ![](images/error3.png)

    ![](images/error4.png)
    
    如图所示，当前`state_`的状态是`2`, 对应`kConnected`, 这是期望的状态，在muduo网络库编写时条件判断写错了，应该是
    ```cpp
    return state_ == kConnected;  // state_ == kConnecting; 错误
    ```

<br>

- 问题二：
    浏览器输入`127.0.0.1:8081`时访问失败，出现以下界面：

    ![](images/error5.png)

    gdb调试没有显示任何问题，但是从打印的日志中我们可以看到响应行的问题，这里应该是`HTTP/1.1 200 OK`而不是`HTTP/1.1 200OK`。

    ![](images/error6.png)

    上面的问题还没有解决，通过`tcpdump`抓包发现400， 用gdb调试也发现了相应的问题，说明解析报文的过程出错了，于是函数返回了`false`

    ![](images/error7.png)

    ![](images/error8.png)

    ```cpp
    if(!context->parseRequest(buf, receiveTime))  // 解析一个http请求
    {
        // 如果解析HTTP报文中出错
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }
    ```
    调试发现`context->parseRequest(buf, receiveTime)`是正确返回`true`，问题在于判断语句后面加个分号。

    ![](images/error9.png)

    ![](images/error10.png)

    修改后，就可以正常显示页面了。

    ![](images/error11.png)

<br>

- 问题三

    正常注册登录后，显示如下的结果：

    ![](images/error12.png)

10. 2025.12.16

    书接上回

    通过gdb调试后发现，客户端显示这个内容是因为在`MenuHandler::handle()`中，服务端并未能找到相应的会话，而这个原因是在`SessionManager::getSession()`中没有将Cookie字段添加到HTTP响应报文中，导致服务端没有把对于的会话ID发送给客户端，客户端下一次发送HTTP请求报文时，其中没有包含相应的Cookie，服务端查不到对于的SessionId，就又创建一个新的会话。

    ![](images/error13.png)

    修复方法，在创建新的会话时调用`setSessionCookie(sessionId, resp);`，这样就能正常的进入菜单了。

    ![](images/error14.png)
    ![](images/error15.png)


- 问题四

    第一次登陆成功后进入菜单，之后再次登陆就登陆失败。原因在与会话下载的判断，如果加载成功，就会返回相应的会话，如果失败，就会返回nullptr，但在加载之后又调用了`session->isExpired()`来判断。如果加载失败，就会出错，因此修改逻辑即可，把`session->isExpired()`改为`!session`
    ```cpp
    if(it != sessions_.end())
    {
        if(!it->second->isExpired())
        {
            return it->second;
        }
        else
        {   // 如果会话过期，则从存储中移除
            sessions_.erase(it);
        }
    }
    return nullptr;
    ```

    
    ![](images/error16.png)

11. 2025.12.17

- 问题五，依然是问题三的显示情况。通过gdb调试后发现，客户端发送的两次请求报文(登录和登录成功后的界面)中，Cookie的SessionId居然是不一样的。

    ![](images/error17.png)

    进一步调试发现，在`LoginHandler()`中，`sessionId`不为空，但是用这个`sessionId`从`storage_`中查找对于会话时找不到，于是`load()`返回了`nullptr`，程序就会认为会话过期了，就重新创建一个新的会话。于是前两次会话的`sessionId`是不一样的。

    ![](images/error18.png)

    在`MenuHandler()`中，`seesionId`同样不为空，并且在`storage_`存放着对对应的键值对，但是在加载过程中显示该会话已经过期了，因此还是返回`nullptr`，按道理来说应该不可能过期的，代码上出现了笔误，在Session的构造函数上。

    ```cpp
    Session::Session(const std::string& sessionId, 
            SessionManager* sessionManger, int maxAge):
    sessionId_(sessionId),
    sessionManager_(sessionManger),
    maxAge_(maxAge)  // maxAge_(maxAge_) 这里出了问题
    ```

    修改后就可以正常进入菜单了。

- 问题五

    ai无限思考中......

    ![](images/error19.png)

    调试发现，落子对于的路由是`/aiBot/move`，但是却找不到对于的路由函数。

    ![](images/error20.png)

    ![](images/error21.png)

    ```cpp
    // 下棋
    httpServer_.Get("/aiBot/move", std::make_shared<AiGameMoveHandler>(this));
    ```
    这里注册路由的时候请求方法是`Get`，但实际请求报文中的请求方法是`POST`，二者不匹配，故找不到对于的路由函数。

    修改，将`.Get()`改成`.Post()`，就可以正常落子了

- 问题六

    AI只会沿着第一行落子，并且判定胜利条件有误

    ![](images/error22.png)

    均已修复，所有功能都正常了。

