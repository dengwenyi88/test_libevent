

event_test : main.o
	cc main.c -I/usr/local/include -L/usr/local/lib -o event_test -levent -levent_pthreads
