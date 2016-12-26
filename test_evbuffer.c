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

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
void* thread_write(void*);
void* thread_read(void*);

int
main(int argc, char **argv)
{
    pthread_t t_write;
    pthread_t t_read;

    evthread_use_pthreads();
    struct evbuffer* buf = evbuffer_new();
    if (NULL == buf) {
        printf("evbuffer_new failed");
        return -1;
    }

    evbuffer_enable_locking(buf,NULL);
    pthread_create(&t_write,NULL,thread_write,(void*)buf);
    pthread_create(&t_read,NULL,thread_read,(void*)buf);

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond,&mutex);
    pthread_mutex_unlock(&mutex);

    //pthread_mutex_destory(&mutex);
    //pthread_cond_destory(&cond);
    evbuffer_free(buf);

    return (0);
}

void* thread_read(void* arg) {

    struct evbuffer* buf = (struct evbuffer*)arg;
    char* read_message;
    size_t read_message_len;
    size_t length = 0;

    while(1) {

        read_message = evbuffer_readln(buf,&read_message_len,EVBUFFER_EOL_CRLF);
        if (NULL != read_message) {
            printf("thread read :%s\n",read_message);
            length += read_message_len;
        }
        sleep(1);
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_wait(&cond,&mutex);
    pthread_mutex_unlock(&mutex);

    while(1){
        read_message = evbuffer_readln(buf,&read_message_len,EVBUFFER_EOL_CRLF);
        if(read_message==NULL)
            break;
        length += read_message_len;
    
    }

    printf("thread read total :%d\n",length);
}

void* thread_write(void* arg) {

    char message_s[]="hello world at %d\n";
    char message[128];
    int message_len;
    int count = 0;
    size_t length;
    size_t total = 0;
    struct evbuffer* buf = (struct evbuffer*)arg;

    while(1) {
        sprintf(message,message_s,++count);
        message_len = strlen(message);
        evbuffer_add(buf,message,message_len);
    
        length = evbuffer_get_length(buf);
        printf("thread write :%d\n",length);
        total += message_len;
        sleep(1);
    }

    pthread_mutex_lock(&mutex);
    pthread_cond_signal(&cond);
    pthread_mutex_unlock(&mutex);
    printf("thread write total :%d\n",total);
}


