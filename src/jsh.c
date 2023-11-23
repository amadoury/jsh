#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include "parser.h"
#include "command.h"

int main(int argc, char *argv[], char *envp[]){

    struct argv_t * arg = malloc(sizeof(struct argv_t));

    int nb_jobs = 0;

    int last_command_return = 0;
    rl_outstream = stderr;

    do {

        fprintf(stderr, "\001\033[32m\002[");
        fprintf(stderr, "%d", nb_jobs);
        fprintf(stderr, "]\001\033[36m\002");
        
        char *pwd = pwd_jsh();

        fprintf(stderr, "%s", pwd);

        fprintf(stderr, "\001\033[00m\002");

        strcat(pwd, "\n");

        char * line = readline("$");
        fprintf(stderr,"test3\n");

        char * l = malloc(sizeof(char) * (strlen(line) + 1)); 
        strcpy(l, line);

        arg = split(line);
        add_history(l);

        if (strcmp(arg->data[0], "cd") == 0){
            if (arg->len == 1){
                last_command_return = cd("");
                if (last_command_return == 1){
                    fprintf(stderr, "error with cd");
                    exit(1);
                }
            }
            else{
                last_command_return = cd(arg->data[1]);
                if (last_command_return == 1){
                    fprintf(stderr, "error with cd");
                    exit(1);
                }
            }
        }
        else if (strcmp(arg->data[0], "pwd") == 0){
            pwd = pwd_jsh();
            fprintf(stderr, pwd);
            last_command_return = pwd == NULL ? 1 : 0;
        }
        else if (strcmp(arg->data[0], "exit") == 0){
            exit_jsh(0);
        }
        else if (strcmp(arg->data[0], "?") == 0){
            fprintf(stderr, "%d",last_command_return);
        }
        else{
            char * path = malloc((10 + strlen(arg->data[0])) * sizeof(char));
            if (path == NULL){
                fprintf(stderr, "error path allocation");
                exit(1);
            }

            strcpy(path, "/usr/bin/");
            strcat(path, arg->data[0]);


            pid_t pids = fork();
            switch (pids)
            {
            case 0 :  
                int res_exec = execl(path, arg->data[1], NULL);
                if (res_exec == -1){
                    perror("errow with exec");
                    exit(1);
                }
                break;
            default:
                wait(pids);
                break;
            }
        }
    }
    while(strcmp(arg->data[0], "exit") != 0);
    
    return 0;
}