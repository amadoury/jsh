#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "parser.h"
#include "command.h"

void func(struct argv_t * arg){
    arg->data = arg->data + 1;
    for(int i = 0; i <= arg->len - 1; ++i){
        if (arg->data[i] == NULL){
            fprintf(stderr, "NULL\n");
        }
        else{
            fprintf(stderr, "%s\n", arg->data[i]);
        }
    }
}


int main(int argc, char *argv[], char *envp[]){

    struct argv_t * arg = malloc(sizeof(struct argv_t));

    int nb_jobs = 0;

    int last_command_return = 0;
    rl_outstream = stderr;
    //char * line;

    do {

        char *pwd = pwd_jsh();
        char *p = malloc(sizeof(char) * 41);
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
        if(line == NULL) break;
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
                        fprintf(rl_outstream, "No such file or directory\n");
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
                    exit_jsh(0);
                }
                else if (arg->len == 2){
                    int val_exit = atoi(arg->data[1]);
                    exit_jsh(val_exit);
                }
                else{
                    fprintf(stderr, "exit has at most two arguments\n");
                }
            }
            else if (strcmp(arg->data[0], "?") == 0){
                fprintf(rl_outstream, "%d\n",last_command_return);
            }
            else{
                char * path = malloc((10 + strlen(arg->data[0])) * sizeof(char));
                if (path == NULL){
                    fprintf(rl_outstream, "error path allocation");
                    exit(1);
                }

                strcpy(path, "/usr/bin/");
                strcat(path, arg->data[0]);

                pid_t pids = fork();
            
                switch (pids)
                {
                case 0 :
                {
                    int r = execv(path, arg->data);
                    if (r == -1){
                        fprintf(stderr,"Unknown command\n");
                    }
                    break;
                }
                    
                default:
                    wait(NULL);
                    break;
                }
            }
        }
    }
    while(arg->len == 0 || strcmp(arg->data[0], "exit") != 0);
    
    return 0;
}
