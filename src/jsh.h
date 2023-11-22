#ifndef JSH_H
#define JSH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXIT_VAL 0
#define MAX_PATH_LENGTH 1024

void exitJSH(int val);
char *pwdJSH();

#endif