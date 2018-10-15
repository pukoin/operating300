CC = gcc
CCFLAGS = -std=gnu99 -pthread -lrt -D_POSIX_C_SOURCE=199309L -g -Wall -Werror

SRC = candykids.c
EXE = candykids

all: candykids

%.o : %.c
	$(CC) -c $(CCFLAGS) $<

candykids: candykids.o bbuff.o stats.o
	$(CC) $(CCFLAGS) -o $(EXE) candykids.o bbuff.o stats.o

clean:
	rm -f core *.o candykids
