#ifndef TCP_SERVER_H
#define TCP_SERVER_H

#include "thread.h"

#include <string.h>

// man inet_aton
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <event2/event.h>
#include <event2/listener.h>

class Tcpserver
{
private:
    Thread *m_threadPool;      // 线程池
    int m_threadNum;           // 线程池中线程的个数
    struct event_base *m_base; // 每个线程中都有一个事件(集合)
public:
    int currentThread; // 当前的线程
private:
    struct evconnlistener *m_listener; // 监听事件

public:
    /// @brief
    /// @param 线程的个数
    Tcpserver(int threadNum = 8);
    ~Tcpserver();

public:
    /// 监听客户端的连接
    /// 当ip为null时,表示允许接听所有地址
    int listen(int port, const char *ip = NULL);

    // 服务器的运行函数
    void start();

public:
    // 客户端的处理函数
    void listenEvent(evutil_socket_t fd, struct sockaddr_in *clientAdr);

protected:
    /// 回调函数
    // 有客户端连接的回调函数,有客户端就会调用,但是不处理,处理交给listenEvent函数
    static void listener_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int, void *);
};

#endif

/*
为啥回调函数中有人喜欢设置成protected权限且为static?

answer:
在C++编程中，将回调函数设置为 protected 并使用 static 关键字具有一些特定的目的和优势。
这些选择通常取决于设计上的需求和对类的封装与访问控制的考虑。
下面分别解释为何要这么做：

# 设置为 protected
- 封装和继承：
protected 访问修饰符使得函数在 类本身及其派生类中 可见，但不对外界可见。
这种方式很有用，尤其是在设计一个应该被继承并在子类中用到的回调函数时。
在库或框架设计中，这可以保证只有那些意图扩展或修改基类行为的派生类能够访问并重写这些回调函数。

- 控制和安全：
使用 protected 可以避免类的使用者直接调用这些函数，这有助于防止误用，确保类的内部状态的完整性和一致性。
通过限制对回调函数的直接访问，开发者可以更安全地管理和维护代码，尤其是在多人协作的项目中。

# 设置为 static
- 回调的兼容性：
在很多编程环境中，回调函数需要是静态的，
因为非静态成员函数依赖于具体对象的上下文（即需要一个对象实例来调用）。
而静态成员函数不需要类实例就可以被调用。
对于很多C/C++库（如libevent），
回调通常不能直接传递非静态成员函数，
因为它们的函数指针类型不兼容。
使用静态成员函数可以解决这个问题。

- 回调时的状态管理：
静态函数不能直接访问类的非静态成员变量或函数。
如果需要在静态回调函数中访问对象的状态，
通常需要通过传递额外的参数
（例如，通过 void* 指针传递对象的 this 指针）来实现。
这种方式允许回调函数与对象的具体实例解耦，
使得设计更加灵活，尤其是在处理多个实例的情况下。

综上所述，将回调函数设置为 protected 和 static
可以为类设计提供更好的封装和灵活性，
同时确保回调机制的安全性和兼容性。
这样的设计在需要类继承或多态性质的复杂系统中尤为常见。
*/
