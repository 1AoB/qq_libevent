#include "thread.h"

Thread::Thread()
{
    // printf("Thread::Thread()\n");
    m_base = event_base_new();
    if (!m_base)
    {
        printf("系统的问题:事件集合event没创建成功...\n");
        exit(1);
    }

    // 创建管道 使用`man 2 pipe`,然后找到下面,会有函数的一些例子,我们直接拷贝
    int pipefd[2];

    if (pipe(pipefd) == -1)
    {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    m_pipeReadFd = pipefd[0];
    m_pipeWriteFd = pipefd[1];

    // 把管道事件添加到m_base的事件集合中
    // 1.我们首先要把管道变成一个事件  EV_PERSIST:表示始终存在(相当于正式录用为员工)
    event_set(&m_pipeEvent, m_pipeReadFd, EV_READ | EV_PERSIST, pipeCb, this);
    // 2.再把事件添加到事件集合中 m_base是事件集合(相当于员工登记)
    event_base_set(m_base, &m_pipeEvent);
    // 3.最后这行代码把m_pipeEvent事件正式添加到事件循环中(相当于员工开始工作)
    event_add(&m_pipeEvent, 0);
}

Thread::~Thread()
{
    // printf("Thread::~Thread()\n");
}

void Thread::start()
{
    // printf("void Thread::start()\n");
    //  创建线程
    int ret = pthread_create(&m_threadID, NULL,
                             worker, this); // statrt->worker->run
    pthread_detach(m_threadID);             // 线程的分离
}

void Thread::run()
{
    // printf("void Thread::run()\n");
    //  printf("线程:[%lu]的开始工作/运行\n", m_threadID);

    // dispatch是一个死循环:用来处理事件集合的
    // 当事件集合为空时,dispatch就会立马返回,
    // 所以要想一直执行,
    // 我们要在初始化时,要令 事件集合(m_base) 不为空
    // 因为一开始也没有任何客户端需要连接,所以我们要搞点事
    // 我们可以:
    // 让主线程与子线程通过管道进行连接,[管道的写端在主线程,管道的读端在子线程]
    event_base_dispatch(m_base);

    event_base_free(m_base);

    // printf("线程:[%lu]的结束工作/运行\n", m_threadID);
}

event_base *Thread::get_base()
{

    return this->m_base;
}

/// @brief 工作函数
/// @param arg 自己
/// @return
/// 注意:静态成员函数只能使用静态成员变量
void *Thread::worker(void *arg)
{
    // printf("void *Thread::worker(void *arg)\n");
    //  在这里我们不能打印m_threadID,因为m_threadID是非静态的,我们可以通过arg参数,把自己传过来,再调用run函数
    //  printf("线程:%的开始工作/运行\n", m_threadID);

    Thread *t = (Thread *)arg;
    t->run();
    return nullptr;
}

void Thread::pipeCb(int, short, void *)
{
    // printf("void Thread::pipeCb(int, short, void *)\n");
}

void Thread::test()
{
    // printf("void Thread::test()\n");
}
