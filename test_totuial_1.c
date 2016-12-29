
#include <sys/types.h>
#include <event2/event-config.h>

#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <event.h>
#include <event2/thread.h>

void callback(int fd,short events,void* arg) {
    
    char buf[512];
    memset(buf,0,512);
    read(fd,buf,sizeof(buf));
    printf("callback data:%s",buf);
}

void signal_callback(int fd,short events,void* arg) {

    struct event *signal = arg;
    struct event_base* base = event_get_base(signal);

    printf("You are Die!\n");
    event_base_loopexit(base,NULL);
}

int main() {

    struct event_base* base;    
    struct event input;
    struct event signal_ev;
    int ret;
    evthread_use_pthreads();

    base = event_base_new();
    /*
    ev = event_new(base,STDIN_FILENO,EV_READ | EV_PERSIST,callback,NULL);
    ret = event_add(ev,NULL); 
    if( ret == -1 ) { 
        return -1;
    } 
    */
    ret = event_assign(&input,base,STDIN_FILENO,EV_READ | EV_PERSIST,callback,NULL);
    if( ret == -1 ) {
        event_base_free(base);
        return -1;
    }
    event_add(&input,NULL);

    event_assign(&signal_ev,base,SIGINT,EV_SIGNAL,signal_callback,&signal_ev);
    event_add(&signal_ev,NULL);

    event_base_dispatch(base); 
    event_base_free(base); 
    return (0); 
}
