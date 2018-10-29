TARGET = locking
OBJS = main.o klock.o list.o

CFLAGS = -Wall -g -std=c99 -Werror -pthread -lrt -D_POSIX_C_SOURCE=199309L
CC = gcc

all: clean $(TARGET)

%.o : %.c
	$(CC) -c $(CFLAGS) $<

$(TARGET): $(OBJS)
	$(CC) $(CFLAGS) $(OBJS) -o $@

test:
	$(CC) $(CFLAGS) klock.c list.c test_graph.c -o test_graph
	./test_graph

clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f test_graph.o test_graph
