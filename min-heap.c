#include <event2/event-config.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>
#include <minheap-internal.h>

void set_random_timeout(struct event* ev);
void min_heap_print(struct min_heap* heap);

int
main(int argc, char **argv)
{
    struct min_heap heap;
    struct event *inserted[1024];
    struct event *e,*last_e;
    int i;

    min_heap_ctor(&heap);
    
    for(i = 0;i < 1024; ++i) {
        inserted[i] = mm_malloc(sizeof(struct event));
        set_random_timeout(inserted[i]);
        min_heap_push(&heap,inserted[i]);
    }

    min_heap_print(&heap);
    printf("heap size :%d\n",min_heap_size(&heap));

    while(1) {
        e = min_heap_pop(&heap);
        if(!e)
            break;
        mm_free(e);
    }

    min_heap_dtor(&heap);
    return (0);
}

void set_random_timeout(struct event* ev) {
    ev->ev_timeout.tv_sec = rand();
    ev->ev_timeout.tv_usec = rand() & 0xfffff;
    ev->ev_timeout_pos.min_heap_idx = -1;
}

void min_heap_print(struct min_heap* heap) {
    
    unsigned i;
    for ( i = 0;i < heap->n; ++i ) {
        printf("i:% 4d,%d,%d\n",i,heap->p[i]->ev_timeout.tv_sec,
                heap->p[i]->ev_timeout.tv_usec);
    }


}



