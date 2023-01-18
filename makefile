CFLAGS += -Wall
CFLAGS += -I /

OBJS = app.o coroutine.o colib.so

app: app.o coroutine.o
	gcc app.o coroutine.o -o app

colib: coroutine.o
	gcc -shared -fPIC -o colib.so coroutine.o

app.o: app.c
	gcc $(CFLAGS) -c app.c -o app.o

coroutine.o: coroutine.c coroutine.h
	gcc $(CFLAGS) -fPIC -c coroutine.c -o coroutine.o

clean:
	rm $(OBJS) app