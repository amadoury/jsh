#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct argv_t{
    char ** data;
    int len;
};

int nb_words(char *);
struct argv_t * split(char *);
void free_argv_data(struct argv_t *);
bool is_redirection(struct argv_t *);

#endif