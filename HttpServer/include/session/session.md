# 会话管理

## 为什么需要会话管理？
- HTTP基于请求/响应模式：所有请求都是相互独立的，无连续性的
- HTTP是无连接的协议（短连接）：限制每次连接只处理一个请求
- HTTP是无状态协议：协议对于事务处理没有记忆能力

对于需要客户端和服务端`多次交互`的网络应用，则必须`记住客户端的状态`，比如购物车，用户登录等。会话就是一个`客户端连续不断地和服务端进行请求/响应的一系列交互`，多次请求间建立关联的方式，称之为会话管理，或者会话跟踪。

## 会话的建立

由于HTTP没有提供任何记住客户端的途径，那么服务器如何建立维护与客户端的会话？

当服务端接收到客户端的首次请求时，服务器初始化一个会话并分配给给会话一个`唯一的标识符`(sessionId)。在以后得请求中，客户端必须将唯一的标识符包含在请求中（请求头），服务器根据次表示符将请求与对应的会话联系起来。
```css
Host: example.com                                         # 必须
User-Agent: Mozilla/5.0 (Windows NT 10.0; Win64; x64)     # 客户端信息
Accept: text/html, application/json                       # 可接受的响应类型
Content-Type: application/json                            # 请求字体的MIME类型
Authorization: Bearer abc123xyz                           # 认证信息
Cookie: sessionId=abc123; username=john                   # **Cookie**
Content-Length: 56                                        
Connection: keep-alive                                    # 连接管理
```
