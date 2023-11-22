CC ?= gcc
CCFLAGS ?= -Wall -g

all: jsh

jsh: src/jsh.c src/parser.o src/cd.o
	$(CC) $(CCFLAGS) -o src/jsh src/jsh.c src/parser.o src/cd.o -lreadline

parser.o: src/parser.c
	$(CC) $(CCFLAGS) -o src/parser.o -c src/parser.c

cd.o: src/cd.c
	$(CC) $(CCFLAGS) -o src/cd.o -c src/cd.c

clean:
	rm -f src/jsh.o src/parser.o