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


struct Node {
    int data;
    TAILQ_ENTRY(Node) node;
};

TAILQ_HEAD(Queue,Node);

struct CircleNode {
    int data;
    CIRCLEQ_ENTRY(CircleNode) node;
};

CIRCLEQ_HEAD(CircleQueue,CircleNode);

#define TAILQ_PRINT(head,headname,field,foreach) do { \
    struct headname* tmp;          \
    foreach(tmp,head,field){ \
        printf("%d ",tmp->data); \
    }                            \
    printf("\n");                \
}while(0)

#define TAILQ_REMOVEALL(head,nodename,field,foreach,removed) do { \
    struct nodename* tmp;            \
    foreach(tmp,head,field){   \
        removed(head,tmp,field);\
        mm_free(tmp);                \
    }                                \
}while(0)



int
main(int argc, char **argv){

    struct Queue queue;
    struct CircleQueue root;
    struct Node* n;
    struct CircleNode* nn;
    int i;

    TAILQ_INIT(&queue);
    for( i=0; i<10; i++ ) {
        n = mm_calloc(1,sizeof (struct Node));
        n->data = i;
        TAILQ_INSERT_TAIL(&queue,n,node);
        TAILQ_PRINT(&queue,Node,node,TAILQ_FOREACH);
    }
    TAILQ_REMOVEALL(&queue,Node,node,TAILQ_FOREACH,TAILQ_REMOVE);

    CIRCLEQ_INIT(&root);
    for( i=0; i< 10;i++){
        nn = mm_malloc(sizeof(struct CircleNode));
        nn->data = i;
        CIRCLEQ_INSERT_TAIL(&root,nn,node);
        TAILQ_PRINT(&root,CircleNode,node,CIRCLEQ_FOREACH);
    }
    TAILQ_REMOVEALL(&root,CircleNode,node,CIRCLEQ_FOREACH,CIRCLEQ_REMOVE);

    return (0);
}




