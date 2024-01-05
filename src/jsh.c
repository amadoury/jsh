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

        // char ** data = split_substitution(arg);

        // for(int i = 0; i < 10; ++i){
        //     printf("%s\n", data[i]);
        // }

        int new_len = arg->len;
        
        struct argv_t *new_arg = build_substitution(arg->data, &new_len, 1);
        // struct argv_t *new_arg = arg;

        new_arg->len = new_len;
        new_arg->esp = arg->esp;
 
        int n_pipes = count_pipes(new_arg->data, new_arg->len);


        if (is_input_well_formed(arg) == 0){
            fprintf(stderr, "error syntax\n");
        }
        else{
            if (n_pipes > 0) {
                char **cmd_pipe = split_pipe(new_arg->data, new_arg->len, n_pipes);
                if (cmd_pipe == NULL){
                    fprintf(stderr, "Error syntax\n");
                }
                else{
                    build_pipe(cmd_pipe, n_pipes);
                    for(int i = 0; i <= n_pipes; ++i){
                        free(cmd_pipe[i]);
                    }
                    free(cmd_pipe);
                }
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
        build_clean(arg);
        free(new_arg);
    }
    return 0;
}
