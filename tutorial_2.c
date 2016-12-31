#include <event2/event-config.h>

#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include <event2/event.h>
#include <event2/util.h>

#ifdef WIN32
#define LOCAL_SOCKETPAIR_AF AF_INET
#else
#define LOCAL_SOCKETPAIR_AF AF_UNIX
#endif

int
main(int argc, char **argv)
{
    int fds[2];
    int ret;

    ret = evutil_socketpair(LOCAL_SOCKETPAIR_AF,SOCK_STREAM,0,fds);
    if( ret < 0 ) {
        printf("evutil_socketpair falt %d!",ret);
        return -1;
    }

    if( fork() ) {
        //parent process
        int count = 0;
        int val;
        close(fds[1]);
        while(1){
            ++count;
            write(fds[0],&count,sizeof count);
            read(fds[0],&val,sizeof val);
            printf("recv data :%d\n",val);
        }

    }else{
        //child process
        int val;
        close(fds[0]);
        while(1){
            sleep(1);
            read(fds[1],&val,sizeof(val));
            printf("recv data :%d\n",val);
            val = -val;
            write(fds[1],&val,sizeof val);
        }
    }

    return (0);
}




