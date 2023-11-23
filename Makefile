CC ?= gcc
CCFLAGS ?= -Wall -g

all: jsh

jsh: src/jsh.c parser.o cd.o
	$(CC) $(CCFLAGS) -o jsh src/jsh.c parser.o cd.o -lreadline

parser.o: src/parser.c
	$(CC) $(CCFLAGS) -o parser.o -c src/parser.c

cd.o: src/cd.c
	$(CC) $(CCFLAGS) -o cd.o -c src/cd.c

clean:
	rm -f src/jsh.o src/parser.o