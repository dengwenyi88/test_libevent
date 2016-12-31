## test_libevent
> 本文档旨在通过简单的例子讲述
从易到难如何使用libevent网络库
每个示例尽量保持简单

### * tutorial_1.c
这个示例讲述小根堆的用法，小根堆在libevent中用于管理
超时事件的时间，根节点为最先超时的事件，其中还包含
libevent中对时间接口的使用

### * tutorial_2.c
这个示例讲述socket_pair的用法，socket pair主要用于
在libevent中事件在线程之间，进程之间的通信，事件通知在
线程间流动,本质上socket pair就是本地成对的套接字

### * tutorial_3.c
这个示例讲述evbuffer的用法，虽然在libevent中不会直接用到
evbuffer，但是了解缓冲区的原理对理解libevent起关键作用，
也说明evbuffer对libevent的性能起关键作用，evbuffer是线程
安全的

### * tutorial_4.c
这个示例主要讲述TAILQ_QUEUE以及CIRCLEQ的主要使用接口
是在libevent中对event的管理上，既要容易对事件进行遍历
又要容易对事件进行删除和增加，是个难题，这里QUEUE数组
提供了支持，不同的队列优先级不同

### * tutorial_5.c
这个示例主要讲述libeve中对锁的使用，结合示例4的队列
实现一个消费者-生产者队列

### * tutorial_6.c
这个示例主要讲述信号事件的使用，在libevent中超时事件
IO事件和信号事件,在使用过程中信号事件和IO事件是互斥的
在信号事件和IO事件中可以计入超时处理
