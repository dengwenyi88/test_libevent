#include <event2/event-config.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

#include <event2/event.h>
#include <event2/util.h>
#include <event2/buffer.h>
#include <event2/thread.h>
#include <evthread-internal.h>
#include <mm-internal.h>
#include <compat/sys/queue.h>
#include <event.h>
#include <signal.h>
#include <event2/bufferevent.h>

void* thread_read(void*);
void  timeout_callback(evutil_socket_t fd,short events,void* arg);
void  signal_callback(evutil_socket_t fd,short events,void* arg);
void  pair_read_cb(struct bufferevent* event,void* arg);
void  pair_error_cb(struct bufferevent* event,short events,void* arg);
void  write_callback(evutil_socket_t fd,short events,void* arg);

//----------------------------------------------
struct Node {
    int index;
    struct event_base* base;

    struct bufferevent* writer;
    CIRCLEQ_ENTRY(Node) next;
};

CIRCLEQ_HEAD(CircleQueue,Node);
struct Node* cur_node;

int
main(int argc, char **argv){
    
    struct event_base* base;
    struct bufferevent* pair[2];
    struct event* tmp_event;
    pthread_t t_pid;
    int ret,i =0;
    struct timeval tv;
    struct CircleQueue queue;
    struct Node* node;

    evthread_use_pthreads();
    CIRCLEQ_INIT(&queue);
    evutil_timerclear(&tv);
    tv.tv_sec = 100;

    //创建四个读任务
    for( i=0;i<4;i++) {
        base = event_base_new();
        ret = bufferevent_pair_new(base,BEV_OPT_THREADSAFE | BEV_OPT_CLOSE_ON_FREE,pair);
        //printf("buffervent_pair_new %d :%d\n",i,ret);
        bufferevent_setcb(pair[1],pair_read_cb,NULL,pair_error_cb,base);
        bufferevent_enable(pair[1],EV_READ | EV_PERSIST);
        bufferevent_enable(pair[0],EV_WRITE | EV_PERSIST);
        bufferevent_set_timeouts(pair[1],&tv,NULL);

        node = mm_malloc(sizeof(struct Node));
        node->index = i;
        node->base = base;
        node->writer = pair[0];
        CIRCLEQ_INSERT_TAIL(&queue,node,next);

        //pthread_create(&t_pid,NULL,thread_read,base);
    }

    base = event_base_new();

    //增加退出监听
    tmp_event = mm_malloc(sizeof(struct event));
    event_assign(tmp_event,base,SIGINT,EV_SIGNAL,signal_callback,tmp_event);
    event_add(tmp_event,NULL);

    //增加超时后的写
    cur_node = CIRCLEQ_FIRST(&queue);
    tmp_event = mm_malloc(sizeof( struct event));
    event_assign(tmp_event,base,-1,EV_PERSIST,write_callback,&queue);
    evutil_timerclear(&tv);
    tv.tv_sec = 1;
    event_add(tmp_event,&tv);

    CIRCLEQ_FOREACH(node,&queue,next){
        pthread_create(&t_pid,NULL,thread_read,node->base);
    }
    //主循环
    event_base_dispatch(base);

    printf("main loop exit\n");
    return 0;
}

void* thread_read(void* arg) {

    struct event_base* base = (struct event_base*)arg;
    int ret = event_base_dispatch(base);
    printf("thread_read exit:%d\n",ret);
    return 0;
}

void timeout_callback(evutil_socket_t fd,short event,void* arg) {
    struct event* ev = (struct event*)arg;
    printf("call timeout_callback!\n");
    mm_free(ev);
}

void signal_callback(evutil_socket_t fd,short event,void* arg) {

    struct event* ev = arg;
    struct event_base* base = event_get_base(ev);
    event_base_loopexit(base,NULL);
}

void pair_read_cb(struct bufferevent* event,void* arg) {
  
    static int count = 0;
    struct event_base* base = (struct event_base*)arg;
    struct event* tmp;
    struct timeval tv;
    char buf[128];

    bufferevent_read(event,buf,128);
    printf("pair_read_cb callback %d!\n",++count);
    evutil_timerclear(&tv);
    tv.tv_sec = 1;
    tmp = mm_malloc(sizeof(struct event));
    event_assign(tmp,base,-1,0,timeout_callback,(void*)tmp);
    event_add(tmp,&tv);
   
}

void pair_error_cb(struct bufferevent* event,short events,void* arg){

}

void write_callback(evutil_socket_t fd,short events,void* arg) {
    
    struct CircleQueue* queue = (struct CircleQueue*)arg;
    printf("write_callback :%d!\n",cur_node->index);
    //cur_node = CIRCLEQ_LOOP_NEXT(queue,cur_node,next);
    cur_node = cur_node->next.cqe_next == (void*)queue ? queue->cqh_first : cur_node->next.cqe_next;
}



