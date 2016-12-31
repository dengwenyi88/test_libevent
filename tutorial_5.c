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

void* thread_run(void*);

struct Node {
    int data;
    TAILQ_ENTRY(Node) node;
};

TAILQ_HEAD(Queue,Node);

struct BlockingQueue {
    struct Queue queue;
    void* empty_cond;
    void* queue_mu;
};

void BQUEUE_INIT(struct BlockingQueue* head) {
    TAILQ_INIT(&head->queue);
    EVTHREAD_ALLOC_LOCK(head->queue_mu,0);
    EVTHREAD_ALLOC_COND(head->empty_cond);   
}

void BQUEUE_PUSH(struct BlockingQueue* head,struct Node*node){

    EVLOCK_LOCK(head->queue_mu,0);
    if( TAILQ_EMPTY(&head->queue) ){
        EVTHREAD_COND_SIGNAL(head->empty_cond);       
    }    
    
    TAILQ_INSERT_TAIL(&head->queue,node,node);
    EVLOCK_UNLOCK(head->queue_mu,0);

}

struct Node* BQUEUE_POP(struct BlockingQueue* head) {
   
    struct Node* node = NULL; 
    EVLOCK_LOCK(head->queue_mu,0);
    while( TAILQ_EMPTY(&head->queue)) {
        EVTHREAD_COND_WAIT(head->empty_cond,head->queue_mu);
    }
    
    node = TAILQ_FIRST(&head->queue);
    TAILQ_REMOVE(&head->queue,node,node);
    EVLOCK_UNLOCK(head->queue_mu,0);   
    return node;
}

void print_qu(struct Queue* q) {

    struct Node* tmp;
    TAILQ_FOREACH(tmp,q,node){
        printf("data :%d\n",tmp->data);
    }
}

void free_qu(struct Queue* root) {

    struct Node* tmp;
    TAILQ_FOREACH(tmp,root,node){
        TAILQ_REMOVE(root,tmp,node);
        mm_free(tmp);
    }
}

int
main(int argc, char **argv)
{

    pthread_t t_pid;
    struct BlockingQueue queue;
    struct Node* tmp;
    evthread_use_pthreads();

    BQUEUE_INIT(&queue);
    pthread_create(&t_pid,NULL,thread_run,(void*)&queue);

    while(1){
        
        tmp = mm_malloc(sizeof(struct Node));
        tmp->data = rand()%10000;
        BQUEUE_PUSH(&queue,tmp);
    }
    printf("main loop exit\n");
    return (0);
}

void* thread_run(void* arg) {
    
    struct BlockingQueue* queue = (struct BlockingQueue*)arg;
    struct Node* tmp;

    while(1) {
        tmp = BQUEUE_POP(queue);
        if(tmp != NULL){
            printf("thread_run :%d\n",tmp->data);
            mm_free(tmp);
        }
    }

    printf("thread_run exit\n");
    return 0;
}



