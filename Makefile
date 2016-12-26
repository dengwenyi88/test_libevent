
CC = gcc
DEBUG = -g -O2
CFLAGS = $(DEBUG) -Wall -c
RM = -rm -rf

M = min-heap

SRCS = test_$(M).c
OBJS = $(patsubst %.c,%.o, $(SRCS))

HEADER_PATH = -I/home/sherlock/libevent-2.0.22 
LIB_PATH = -L/usr/local/lib

LIBS = -levent -levent_pthreads -lpthread
TARGET = test_$(M)

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@ $(LIB_PATH) $(LIBS)

$(OBJS) :%.o :%.c
	$(CC) $(CFLAGS) $< -o $@ $(HEADER_PATH)

clean:
	$(RM) $(TARGET) *.o
