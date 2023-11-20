CC ?= gcc
CCFLAGS ?= -Wall -g

all: jsh

jsh: jsh.c jsh.o
	$(CC) $(CCFLAGS) jsh.c -o jsh 

clean:
	rm -f jsh jsh.o