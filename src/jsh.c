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
    // int old_stdin = dup(0);

    while (1) {
        // if(test == 1) printf("old_stdin %d\n", old_stdin);
        // dup2(old_stdin, 0);
        // close(old_stdin);
        // old_stdin = dup(0);
        
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

        // char ***all_args = malloc(sizeof(char **) * arg->len);
        // int *all_args_len = malloc(sizeof(int) * arg->len);
        
        // struct argv_t *new_arg = build_substitution(arg->data, &new_len, 0, arg->all_fifo, all_args, all_args_len);
        struct argv_t *new_arg = build_substitution(arg->data, &new_len, 0, arg->all_fifo);
        new_arg->esp = 0;


        // for(int i = 0 ; i < new_arg->nb_fifo ; ++i){
        //     printf("substitution %d :\n\n", i);
        //     for(int j = 0 ; j < all_args_len[i] ; ++j){
        //         printf("%s\n", all_args[i][j]);
        //     }
        // }
        // printf("\nmain command :\n");
        // for(int j = 0 ; j < new_arg->len ; ++j){
        //     printf("%s\n", new_arg->data[j]);
        // }
        
        
        // for(int i = 0 ; i < new_arg->nb_fifo ; ++i){
        //     if(!fork()){
        //         int fd = open(new_arg->all_fifo[i], O_WRONLY);
        //         if(fd == -1){
        //             perror("erreur open");
        //             exit(1);
        //         }
        //         printf("TTTTTTTTTTTTTTTTTTTTTTTTTT\n");
        //         dup2(fd, 1);
        //         close(fd);

        //         struct argv_t *arg_sub = malloc(sizeof(struct argv_t));
        //         // arg_sub->data = all_args[i];
        //         // arg_sub->len = all_args_len[i];
        //         // execute_command(arg_sub);
        //         exit(0);
        //     }
        // }
        
        

        
        // struct argv_t *new_arg = arg;

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
            // printf("fifo remove %s\n", new_arg->all_fifo[i]);
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
