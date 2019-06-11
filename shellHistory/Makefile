## This is a simple Makefile with lots of comments 
## Check Unix Programming Tools handout for more info.

# Define what compiler to use and the flags.
CC=cc
CXX=CC
CCFLAGS= -g -std=c99 -D_POSIX_C_SOURCE=200809L -Wall -Werror


# Compile all .c files into .o files
# % matches all (like * in a command)
# $< is the source file (.c file)
shell2.o : shell2.c
	$(CC) -c $(CCFLAGS) $<



shell: shell2.o 
	$(CC) -o shell2 shell2.o $(CCFLAGS)


clean:
	rm -f core *.o shell2 