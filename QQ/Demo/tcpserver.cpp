#include "tcpserver.h"

Tcpserver::Tcpserver(int threadNum)
{
    printf("Tcpserver::Tcpserver(int threadNum)\n");
    // 1.初始化m_base事件集合
    m_base = event_base_new();
    if (!m_base)
    {
        printf("g!事件集合系统都创建失败了!\n");
        exit(1);
    }
    // 2.判断一下传参是否合理
    if (threadNum <= 0)
    {
        printf("g!谁在线程池中传了0个线程?\n");
        exit(1);
    }
    // 3.初始化线程池
    m_threadNum = threadNum;
    m_threadPool = new Thread[m_threadNum];

    currentThread = 0;
}

Tcpserver::~Tcpserver()
{
    printf("Tcpserver::~Tcpserver()\n");
}

int Tcpserver::listen(int port, const char *ip)
{
    printf("begin_int Tcpserver::listen(int port, const char *ip)\n");
    struct sockaddr_in sin;

    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(port);
    if (NULL != ip)
    {
        inet_aton(ip, &(sin.sin_addr));
    }

    m_listener = evconnlistener_new_bind(m_base,
                                         listener_cb,
                                         this,
                                         LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, -1,
                                         (struct sockaddr *)&sin,
                                         sizeof(sin));

    if (!m_listener)
    {
        printf("stderr!!! could nbot create a listener!\n");
        return 1;
    }

    // 开启线程池
    printf("=========================欢迎使用线程池======================\n");
    for (int i = 0; i < m_threadNum; i++)
    {
        m_threadPool[i].start();
        printf("[%lu]线程开始运行\n", m_threadPool[i].getThreadID());
    }
    printf("=============================================================\n");

    printf("end_int Tcpserver::listen(int port, const char *ip)\n");
    return 0;
}

void Tcpserver::start()
{
    printf("void Tcpserver::start()\n");
    // 开始监听
    event_base_dispatch(m_base); // dispatch就是死循环

    evconnlistener_free(m_listener);

    event_base_free(m_base);
}

// 客户端的处理函数
void Tcpserver::listenEvent(evutil_socket_t fd, sockaddr_in *clientAddr)
{
    // printf("void Tcpserver::listenEvent(evutil_socket_t fd, sockaddr_in *clientAddr)\n");
    //  如何做连接的处理

    char *ip = inet_ntoa(clientAddr->sin_addr);
    uint16_t port = ntohs(clientAddr->sin_port);

    printf("线程:[%lu]开始监听客户端:[%s]\n", this->m_threadPool[currentThread].getThreadID(), ip);

    // 线程中的轮询操作
    struct event_base *base = m_threadPool[this->currentThread].get_base(); // 获得线程中的事件集合
    this->currentThread = (this->currentThread + 1) % m_threadNum;          // 轮询取余的操作

    struct bufferevent *bev;
    bev = bufferevent_socket_new(base, fd, BEV_OPT_CLOSE_ON_FREE);
    if (!bev)
    {
        printf("Error constructing bufferevent\n");
        event_base_loopbreak(base);
    }

    // 设置bev的可读可写异常事件的回调函数,以及这些回调函数需要的参数
    //  bufferevent_setcb(bev, conn_read, conn_write, conn_eventcb, NULL);
    //  bufferevent_enable(bev, EV_READ);
    //  bufferevent_enable(bev, EV_WRITE);

    // bufferevent_write(bev, MESSAGE, strlen(MESSAGE));//往这个bev里面写东西
}

// 服务器主线程监听客户端的连接
void Tcpserver::listener_cb(struct evconnlistener *,
                            evutil_socket_t fd,
                            struct sockaddr *clientAddr,
                            int,
                            void *data)
{
    // printf("void Tcpserver::listener_cb\n");
    printf("有客户局端请求连接\n");
    Tcpserver *p = (Tcpserver *)data;
    p->listenEvent(fd, (struct sockaddr_in *)clientAddr);
}
