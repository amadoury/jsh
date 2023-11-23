#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "parser.h"
#include "command.h"


int main(int argc, char *argv[]){

    struct argv_t * arg = malloc(sizeof(struct argv_t));

    int nb_jobs = 0;

    char * last_command_result = "";

    do {
        rl_outstream = stderr;

        char *pwd = pwd_jsh();
        
        fprintf(rl_outstream, "\001\033[32m\002[%d]\001\033[36m\002");
        if(strlen(pwd) >= 26){
            fprintf(rl_outstream, "...%s", pwd+strlen(pwd)-23);
        }
        else{
            fprintf(rl_outstream, "%s", pwd);
        }

        strcat(pwd, "\n");

        fprintf(rl_outstream, "\001\033[00m\002");

        char * line = readline("$");
        char * l = malloc(sizeof(char) * (strlen(line) + 1)); 
        strcpy(l, line);

        arg = split(line);
        add_history(l);

        if (strcmp(arg->data[0], "cd") == 0){
            if(arg->len == 1){
                if (cd("") == 1){
                    fprintf(stderr, "error with cd");
                    exit(1);
                }
            }
            if (cd(arg->data[1]) == 1){
                fprintf(stderr, "error with cd");
                exit(1);
            }
        }
        else if (strcmp(arg->data[0], "pwd") == 0){
            fprintf(stderr, pwd);
            last_command_result = pwd;
        }
        else if (strcmp(arg->data[0], "exit") == 0){
            exit_jsh(0);
        }
        else if (strcmp(arg->data[0], "?") == 0){
            fprintf(stderr, last_command_result);
        }
        else{
            char * path = malloc((10 + strlen(arg->data[0])) * sizeof(char));
            if (path == NULL){
                fprintf(stderr, "error path allocation");
                exit(1);
            }

            strcpy(path, "/usr/bin/");
            strcat(path, arg->data[0]);
            switch (fork())
            {
            case 0 :  
                execv(path, arg->data[1]);
                break;
            default:
                wait(NULL);
                break;
            }

        }
    }
    while(strcmp(arg->data[0], "exit") != 0);
    
    return 0;
}
