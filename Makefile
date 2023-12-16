CC ?= gcc
CCFLAGS ?= -Wall -g

all: jsh

jsh: src/jsh.c parser.o command.o
	$(CC) $(CCFLAGS) -o jsh src/jsh.c parser.o command.o -lreadline

parser.o: src/parser.c
	$(CC) $(CCFLAGS) -o parser.o -c src/parser.c

command.o: src/command.c
	$(CC) $(CCFLAGS) -o command.o -c src/command.c

clean:
	 rm -f jsh.o parser.o command.ohi
