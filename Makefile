CC = gcc
CFLAGS = -Wall -lpthread -fPIC -g

.PHONY: build
all: libscheduler.so
build: libscheduler.so

libscheduler.so: so_scheduler.o priorityqueue.o
	$(CC) -shared -o $@ $^ -g

so_scheduler.o: so_scheduler.c
	$(CC) $(CFLAGS) -o $@ -c $<

priorityqueue.o: priorityqueue.c
	$(CC) $(CFLAGS) -o $@ -c $<

.PHONY: clean
clean:
	rm priorityqueue.o so_scheduler.o libscheduler.so
