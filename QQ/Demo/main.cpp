#include <iostream>
#include "thread.h"
#include "tcpserver.h"

int main()
{
    // Thread *t1 = new Thread;
    // Thread *t2 = new Thread;

    // t1->start();
    // t2->start();

    // while (1)
    // {
    //     printf("[main.c]\n");
    //     sleep(2);
    // }

    Tcpserver *t = new Tcpserver(8); // 只调用构造函数

    t->listen(8000); // 调用了listen函数，此时线程池将会被初始化，线程池里面的每个线程都是用来监听8000这个端口的

    t->start(); // 死循环开始等待客户端的连接

    return 0;
}
