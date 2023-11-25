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

    do {

        char *pwd = pwd_jsh();
        
        fprintf(rl_outstream, "\001\033[32m\002[%d]\001\033[36m\002", nb_jobs);
        if(strlen(pwd) >= 26){
            fprintf(rl_outstream, "...%s", pwd+strlen(pwd)-23);
        }
        else{
            fprintf(rl_outstream, "%s", pwd);
        }

        strcat(pwd, "\n");

        fprintf(rl_outstream, "\001\033[00m\002");

        char * line = readline("$ ");
        char * l = malloc(sizeof(char) * (strlen(line) + 1)); 
        strcpy(l, line);
        add_history(l);

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
                fprintf(stderr, "%s\n",pwd);
                last_command_return = (pwd == NULL) ? 1 : 0;
            }
            else if (strcmp(arg->data[0], "exit") == 0){
                exit_jsh(0);
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
                    int r = execv(path, arg->data);
                    if (r == -1){
                        perror("error execv");
                    }
                    break;
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
