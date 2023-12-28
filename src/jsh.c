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

        int n_pipes = count_pipes(arg);

        if (n_pipes > 0) {
            char **cmd_pipe = split_pipe(arg, n_pipes);
            build_pipe(cmd_pipe, n_pipes);
            free(cmd_pipe);
        } else {
            if (arg->len != 0) {
                index_redirec = is_redirection(arg);
                if (strcmp(arg->data[0], "cd") == 0) {
                    build_cd(arg);
                } else if (strcmp(arg->data[0], "pwd") == 0 && !index_redirec) {
                    build_pwd();
                } else if (strcmp(arg->data[0], "exit") == 0) {
                    build_exit(arg);
                } else if (strcmp(arg->data[0], "jobs") == 0) {
                    build_jobs(arg);
                } else if (strcmp(arg->data[0], "kill") == 0 && strcmp(arg->data[1], "-l") != 0) {
                    build_kill(arg);
                } else if (strcmp(arg->data[0], "?") == 0) {
                    build_interogation();
                } else {
                    build_external(arg);
                }
            
                build_clean(arg);
            }
        }
    }

    return 0;
}
