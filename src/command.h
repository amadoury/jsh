#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *last_path;

void exit_jsh(int);

int cd(const char *);

char *pwd_jsh();


#endif