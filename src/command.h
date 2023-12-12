#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void exit_jsh(int);
 
int cd(const char *);
 
char * pwd_jsh();
 
void redirection(struct argv_t *, int *, int, int, int,int);

void add_job(int, char *);

void remove_jobs();

void add_job_to_remove(pid_t);

void remove_invalid_command();

void print_jobs();

int get_nb_jobs();

void signaux();

void commande_arr_plan(struct argv_t *);

#endif