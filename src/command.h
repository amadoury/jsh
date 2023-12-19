#ifndef COMMAND_H
#define COMMAND_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void exit_jsh(int);
 
int cd(const char *);
 
char * pwd_jsh();
 
int redirection(int *, char *, int, int);

void do_read_or_write_to_file(int, int, char *, char *, int);

void add_job(int, char *);

void remove_jobs(int);

void add_job_to_remove(pid_t);

void remove_invalid_command();

void print_jobs();

int get_nb_jobs();

int set_nb_jobs(int);

void signaux();

void activate_sig();

int kill_job(int , int);

#endif