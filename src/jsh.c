// #include "cd.h"
// #include "exit.h"
// #include "jsh.h"
// #include "pwd.h"
// #include "prompt.h"

#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "parser.h"
#include "command.h"



int main(int argc, char *argv[]){

    struct argv_t * arg = malloc(sizeof(struct argv_t));

    int nb_jobs = 0;

    char * lastCommandResult = "";

    do {
        rl_outstream = stderr;

        
        fprintf(stderr, "\001\033[32m\002[");
        fprintf(stderr, "%d", nb_jobs);
        fprintf(stderr, "]\001\033[36m\002");
        

        char *pwd = pwdJSH();
        fprintf(stderr, pwd);
        fprintf(stderr, "\001\033[00m\002");

        strcat(pwd, "\n");

        char * line = readline("$");
        char * l = malloc(sizeof(char) * (strlen(line) + 1)); 
        strcpy(l, line);

        arg = split(line);
        add_history(l);

        if (strcmp(arg->data[0], "cd") == 0){
            if (cd(arg->data[1]) == 1){
                fprintf(stderr, "error with cd");
                exit(1);
            }
            lastCommandResult = "";
        }
        else if (strcmp(arg->data[0], "pwd") == 0){
            fprintf(stderr, pwd);
            lastCommandResult = pwd;
        }
        else if (strcmp(arg->data[0], "exit") == 0){
            exitJSH(0);
        }
        else if (strcmp(arg->data[0], "?") == 0){
            fprintf(stderr, lastCommandResult);
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
