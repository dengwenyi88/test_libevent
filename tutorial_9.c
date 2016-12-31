#include <event2/event-config.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>

#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <event2/thread.h>

void listener_cb(struct evconnlistener* listen,evutil_socket_t fd,struct sockaddr* addr,int socklen,void* arg);
void socket_read_cb(struct bufferevent* event,void* arg);
void socket_error_cb(struct bufferevent* event,short events,void* arg);

int
main(int argc, char **argv)
{
	struct event_base *base;
    struct sockaddr_in addr;

#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;

	wVersionRequested = MAKEWORD(2, 2);

	(void)WSAStartup(wVersionRequested, &wsaData);
#endif
    evthread_use_pthreads();
    memset(&addr,0,sizeof(struct sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port   = htons(8888);
	base = event_base_new();

    struct evconnlistener* listener  = evconnlistener_new_bind(
            base,
            listener_cb,
            base,
            LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE | LEV_OPT_THREADSAFE,
            10,(struct sockaddr*)&addr,sizeof(struct sockaddr_in));
	
    event_base_dispatch(base);
    evconnlistener_free(listener);
    event_base_free(base);

	return (0);
}

void listener_cb(struct evconnlistener* listener,evutil_socket_t fd,
        struct sockaddr* sock,int socklen,void* arg) {
    
    struct event_base* base = (struct event_base*)arg;
    struct bufferevent* event = bufferevent_socket_new(
            base,fd,BEV_OPT_CLOSE_ON_FREE);

    bufferevent_setcb(event,socket_read_cb,NULL,socket_error_cb,NULL);
    bufferevent_enable(event, EV_READ | EV_PERSIST);
}

void socket_read_cb(struct bufferevent* event,void* arg) {
    char msg[4096];
    size_t len = bufferevent_read(event,msg,sizeof(msg)-1);
    msg[len] = '\0';

    printf("recv data :%s",msg);
    
    //sprintf(msg,"%s","hello world!\n");
    bufferevent_write(event,msg,strlen(msg));

}

void socket_error_cb(struct bufferevent* event,short events,void* arg) {

    bufferevent_free(event);
}



