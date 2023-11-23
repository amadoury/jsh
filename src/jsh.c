#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include "parser.h"


int main(int argc, char *argv[]){

    struct argv_t * arg = malloc(sizeof(struct argv_t));

    do {
        rl_outstream = stderr;

        char * line = readline(">$:");

        arg = split(line);
        add_history(line);

        if (strcmp(arg->data[0], "cd") == 0){
            if (cd(arg->data[1]) == 1){
                fprintf(stderr, "error with cd");
                exit(1);
            }
        }
        else if (strcmp(arg->data[0], "pwd") == 0){
            //do pwd
        }
        else if (strcmp(arg->data[0], "exit") == 0){
            //do exit
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
