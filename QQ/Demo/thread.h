#ifndef THREAD_H
#define THREAD_H

// man 3 sleep
#include <unistd.h>

// man pthread_create
#include <pthread.h>

// man 2 exit
#include <stdlib.h>
// libevent不能用man查看我不知道为啥
#include <event.h>
#include <event2/event.h> //这两个是一样的

/// @brief 线程类
class Thread
{
private:
    struct event_base *m_base;
    pthread_t m_threadID;

    int m_pipeReadFd;         // 管道的读端
    int m_pipeWriteFd;        // 管道的写端
    struct event m_pipeEvent; // 管道事件

public:
    pthread_t getThreadID()
    {
        return this->m_threadID;
    }

public:
    Thread();
    ~Thread();

public:
    /// @brief 线程的运行函数
    void start();

    /// @brief 线程的逻辑函数
    void run();

public:
    // 获得该线程的事件集合
    struct event_base *get_base();

protected:
    /// 线程的处理函数(工作函数)
    // void *(*start_routine)(void *);
    static void *worker(void *);

    static void pipeCb(int, short, void *);

private:
    /// @brief 什么用都没有,临时测试用的
    void test();
};

#endif