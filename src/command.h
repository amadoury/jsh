#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>


void exitJSH(int);

int cd(const char *);

char *pwdJSH();


#endif