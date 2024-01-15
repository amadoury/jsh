#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include "parser.h"
#include "command.h"
#include "build.h"

char *pwd;
int last_command_return = 0;
int index_redirec;
char *line;
char *l;

int main(int argc, char *argv[], char *envp[]) {
    signaux();

    struct argv_t *arg;
    rl_outstream = stderr;

    int test = 0;

    while (1) {
        
        remove_jobs(1, -1);
        
        char *prompt = build_prompt();

        line = readline(prompt);

        free(prompt);

        if (line == NULL) {
            exit_jsh(last_command_return);
        }

        l = malloc(sizeof(char) * (strlen(line) + 1));
        strcpy(l, line);
        add_history(l);


        arg = split(line);


        int new_len = arg->len;

        struct argv_t *new_arg = build_substitution(arg->data, &new_len, 0, arg->all_fifo);
        new_arg->esp = 0;


        new_arg->len = new_len;
        new_arg->esp = arg->esp;
 
        int n_pipes = count_pipes(new_arg->data, new_arg->len);


        if (is_input_well_formed(arg) == 0){
            fprintf(stderr, "error syntax\n");
        }
        else{
            if (n_pipes > 0) {
                build_pipe(new_arg, n_pipes);
            }
            else{
                if (new_len != 0) {
                    index_redirec = is_redirection(new_arg->data, new_len);
                    if (strcmp(new_arg->data[0], "cd") == 0) {
                        build_cd(new_arg);
                    } else if (strcmp(new_arg->data[0], "pwd") == 0 && !index_redirec) {
                        build_pwd();
                    } else if (strcmp(new_arg->data[0], "exit") == 0) {
                        build_exit(new_arg);
                    } else if (strcmp(new_arg->data[0], "jobs") == 0) {
                        build_jobs(new_arg);
                    } else if (strcmp(new_arg->data[0], "kill") == 0 && strcmp(new_arg->data[1], "-l") != 0) {
                        build_kill(new_arg);
                    } else if (strcmp(new_arg->data[0], "?") == 0) {
                        build_interogation();
                    }
                    else if (strcmp(new_arg->data[0], "fg") == 0)
                    {
                        do_fg(new_arg);
                        last_command_return = 0;
                        tcsetpgrp(STDIN_FILENO, getpid());
                        tcsetpgrp(STDOUT_FILENO, getpid());
                    }
                    else if (strcmp(new_arg->data[0], "bg") == 0)
                    {
                        do_bg(new_arg);
                        last_command_return = 0;
                    }
                    else
                    {
                        
                        build_external(new_arg);
                    }
                }
            }
        }
        for(int i = 0 ; i < new_arg->nb_fifo ; ++i){
            remove(new_arg->all_fifo[i]);
            test = 1;
        }
        // free(new_arg->data[new_arg->len]);
        build_clean(arg, new_arg->nb_fifo);
        // for(int i = 0 ; i < arg->nb_fifo ; ++i){
        //     free(arg->all_fifo[i]);
        // }
        // free(arg->all_fifo);

        // for(int i = 0 ; i < new_arg->len ; ++i){
        //     free(new_arg->data[i]);
        // }
        // free(new_arg->data);
        free(new_arg);

    }
    return 0;
}
