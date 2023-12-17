#ifndef PARSER_H
#define PARSER_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>

struct argv_t
{
    char **data;
    int len;
    int esp;
};

int nb_words(char *);

struct argv_t *split(char *);

int is_redirection(struct argv_t *);
int which_redirection_str_is(char *);
int which_redirection(struct argv_t *);
int nb_direction(struct argv_t *);
struct argv_t * data_cmd(struct argv_t *,int);

#endif