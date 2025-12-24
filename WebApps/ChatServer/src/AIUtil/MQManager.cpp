#include "../../include/AIUtil/MQManager.h"

// ---------------------MQManager---------------------
MQManager::MQManager(size_t poolSize): 
    poolSize_(poolSize),
    counter_(0)
{
    for(size_t i = 0; i < poolSize_; ++i){
        auto conn = std::make_shared<MQConn>();
        // 创建对应的AmqClient::Channel
        conn->channel = AmqpClient::Channel::Create("localhost", 5672, "guest", "guest", "/");
        // exclusive use
        pool_.push_back(conn);
    }
}

void MQManager::publish(const std::string& queue, const std::string& msg)
{
    // std::atomic<size_t> counter_;
    size_t index = counter_.fetch_add(1) % poolSize_;  // 原子地递增并返回旧值
    /*  以轮询的操作返回一个连接的索引，并且能保证线程安全

        .fetch_add(1): // 原子地递增并返回旧值
        比如poolSize_ = 4, counter_ = 0, 先返回0，然后自增
        如果counter_ = 3, 先返回3，自增后 % 4 = 0，即
        按照0 1 2 3 0 1 2 3 ... 轮询

        std::vector<std::shared_ptr<MQManager::MQConn>> MQManager::pool_
    
        struct MQConn {
            AmqpClient::Channel::ptr_t channel;
            std::mutex mtx;
        };
    */
    auto& conn = pool_[index];
    std::lock_guard<std::mutex> lock(conn->mtx);
    auto message = AmqpClient::BasicMessage::Create(msg);  // 创建消息对象
    conn->channel->BasicPublish("", queue, message); 
    /*  发布消息
        "", 交换机的名称
        queue, 路由键
        message, 消息对象
    */
}

// ---------------------RabbitMQManager---------------------
void RabbitMQThreadPool::start()
{
    for(int i = 0; i < thread_num_; ++i)
    {
        workers_.emplace_back(&RabbitMQThreadPool::worker, this, i);
    }
}

void RabbitMQThreadPool::shutdown()
{
    stop_ = true;
    for(auto& t: workers_)
    {
        if(t.joinable())
        {
            t.join();
        }
    }
}

void RabbitMQThreadPool::worker(int id)
{
    try
    {
        auto channel = AmqpClient::Channel::Create(rabbitmq_host_, 5672, "guest", "guest", "/");
        channel->DeclareQueue(queue_name_, false, true, false, false);
        /* 声明(创建一个)AMQP队列
            queue_name_: 队列名称
        */
        std::string consumer_tag = channel->BasicConsume(queue_name_, "", true, false, false);

        channel->BasicQos(consumer_tag, 1);

        while(!stop_){
            AmqpClient::Envelope::ptr_t env;
            bool ok = channel->BasicConsumeMessage(consumer_tag, env, 500);  // 500ms 
            if(ok && env)
            {
                std::string msg = env->Message()->Body();  // 消息本体
                handler_(msg);  // 处理消息(把sql语句给到mysql进行处理？)
                /* 实际上绑定的回调函数就是
                    void executeMysql(const std::string sql)
                    {
                        http::MysqlUtil mysqlUtil_;
                        mysqlUtil_.executeUpdate(sql);
                    }
                */
                channel->BasicAck(env);  // 手动确认
            }
        }
        channel->BasicCancel(consumer_tag);
    }
    catch(const std::exception& e)
    {
        std::cerr << "Thread " << id << " exception: " << e.what() << '\n';
    }
    
}