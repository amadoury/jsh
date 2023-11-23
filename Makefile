CC ?= gcc
CCFLAGS ?= -Wall -g

all: jsh

 jsh: src/jsh.c parser.o command.o
	$(CC) $(CCFLAGS) -o jsh src/jsh.c src/parser.o src/command.o -lreadline

parser.o: src/parser.c
	$(CC) $(CCFLAGS) -o parser.o -c src/parser.c

cd.o: src/command.c
	$(CC) $(CCFLAGS) -o command.o -c src/command.c

clean:
	 rm -f src/jsh.o parser.o command.o