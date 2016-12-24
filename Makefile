
CC = gcc
DEBUG = -g -O2
CFLAGS = $(DEBUG) -Wall -c
RM = -rm -rf

SRCS = min-heap.c
OBJS = $(patsubst %.c,%.o, $(SRCS))

HEADER_PATH = -I/home/sherlock/libevent-2.0.22 
LIB_PATH = -L/usr/local/lib

LIBS = -levent
TARGET = test_libevent

$(TARGET) : $(OBJS)
	$(CC) $^ -o $@ $(LIB_PATH) $(LIBS)

$(OBJS) :%.o :%.c
	$(CC) $(CFLAGS) $< -o $@ $(HEADER_PATH)

clean:
	$(RM) $(TARGET) *.o
