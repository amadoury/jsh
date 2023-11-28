#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <stdlib.h>
#include "parser.h"
#include "command.h"

#define SIZE_STR_INPUT 41

int main(int argc, char *argv[], char *envp[]){

    struct argv_t * arg = malloc(sizeof(struct argv_t));

    int nb_jobs = 0;

    int last_command_return = 0;
    rl_outstream = stderr;

    do {

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
        if (line == NULL){
            exit_jsh(last_command_return);
        }
    
        free(p);
        free(pwd);
        if(line == NULL) exit(last_command_return);
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
                        fprintf(rl_outstream, "bash: cd: NONEXISTENT: No such file or directory\n");
                    }
                }
                else{
                    last_command_return = cd(arg->data[1]);
                    if (last_command_return == 1){
                        fprintf(rl_outstream, "bash: cd: NONEXISTENT: No such file or directory\n");
                    }
                }
            }

            else if (strcmp(arg->data[0], "pwd") == 0){
                pwd = pwd_jsh();
                fprintf(stdout, "%s\n",pwd);
                last_command_return = (pwd == NULL) ? 1 : 0;
            }

            else if (strcmp(arg->data[0], "exit") == 0){
                if (arg->len == 1){
                    exit_jsh(last_command_return);
                }
                else if (arg->len == 2){
                    int val_exit = atoi(arg->data[1]);
                    exit_jsh(val_exit);
                }
                else{
                    fprintf(stderr, "-bash: exit: too many arguments\n");
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
                        execv(arg->data[0], arg->data);
                    }
                    else{
                        int r = execvp(arg->data[0], arg->data);
                        if (r == -1){
                            fprintf(stdout,"Unknown command\n");
                        }
                    }
                    break;
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
    }
    while(arg->len == 0 || strcmp(arg->data[0], "exit") != 0);
    
    return 0;
}
