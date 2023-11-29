#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "parser.h"
#include "command.h"

#define SIZE_STR_INPUT 100

int main(int argc, char *argv[], char *envp[]){

    struct argv_t * arg;

    int nb_jobs = 0;

    int last_command_return = 0;
    rl_outstream = stderr;

    while(1){

        char *pwd = pwd_jsh();
        char *p = malloc(sizeof(char) * SIZE_STR_INPUT);
        if(p == NULL){
            fprintf(stdout, "error malloc");
            exit(1);
        }
        *p = '\0';
        strcat(p, "\001\033[32m\002[");
        char *nb_jobs_tab = malloc(sizeof(char) * 2);
        nb_jobs_tab[0] = nb_jobs + '0';
        nb_jobs_tab[1] = '\0';
        strcat(p, nb_jobs_tab);
        free(nb_jobs_tab);
        strcat(p, "]\001\033[36m\002");

        if(strlen(pwd) >= 26){
            strcat(p, "...");
            strcat(p, pwd+strlen(pwd)-23);
        }
        else{
            strcat(p, pwd);
        }

        strcat(p, "\001\033[00m\002$ ");

        char * line = readline(p);
        free(p);
        free(pwd);
        if (line == NULL){
            exit_jsh(last_command_return);
        }
    

        char * l = malloc(sizeof(char) * (strlen(line) + 1)); 
        strcpy(l, line);
        add_history(l);
        free(l);

        arg = split(line);
    
        if (arg->len != 0){
            if (strcmp(arg->data[0], "cd") == 0){
                if (arg->len == 1){
                    last_command_return = cd(NULL);
                    if (last_command_return == 1){
                        fprintf(rl_outstream, "No such file or directory\n");
                    }
                }
                else{
                    last_command_return = cd(arg->data[1]);
                    if (last_command_return == 1){
                        fprintf(rl_outstream, "bash: cd: %s: No such file or directory\n", arg->data[0]);
                    }
                }
            }

            else if (strcmp(arg->data[0], "pwd") == 0){
                pwd = pwd_jsh();
                fprintf(stdout, "%s\n",pwd);
                last_command_return = (pwd == NULL) ? 1 : 0;
                free(pwd);
            }

            else if (strcmp(arg->data[0], "exit") == 0){
                if (arg->len == 1){
                    free(arg->data);
                    free(arg);
                    free(line);
                    exit_jsh(last_command_return);
                }
                else if (arg->len == 2){
                    int val_exit = atoi(arg->data[1]);
                    free(arg->data);
                    free(arg);
                    free(line);
                    exit_jsh(val_exit);
                }
                else{
                    fprintf(stdout, "exit has at most two arguments\n");
                }
            }

            else if (strcmp(arg->data[0], "?") == 0){
                fprintf(stdout, "%d\n",last_command_return);
                last_command_return = 0;
            }

            else{

                pid_t pids = fork();
                int status;
                switch (pids)
                {
                case 0 :
                {
                    if (arg->data[0][0] == '.' || arg->data[0][0] == '/'){
                        int r = execv(arg->data[0], arg->data);
                        if (r == -1){
                            fprintf(stderr,"Unknown command\n");
                        } 
                    }
                    else{
                        int r = execvp(arg->data[0], arg->data);
                        if (r == -1){
                            fprintf(stderr,"Unknown command\n");
                        }
                    }
                    free(arg->data);
                    free(arg);
                    free(line);
                    return 0;
                }
                    
                default:
                    wait(&status);
                    if (WIFEXITED(status)){
                        last_command_return = WEXITSTATUS(status);
                    }
                    else {
                        last_command_return = 1;
                    }
                    break;
                }
            }
        }
        free(arg->data);
        free(arg);
        free(line);
    }
    return 0;
}
