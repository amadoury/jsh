#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct argv_t{
    char ** data;
    int len;
};

char ** parse_line(char *);
int nb_words(char *);
struct argv_t * split(char *);

#endif