#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>     
#include "parser.h"

#define EXIT_VAL 0
#define MAX_PATH_LENGTH 4096

char *last_path;

void exit_jsh(int val)
{
    exit(val);
}

char *pwd_jsh()
{
    char *pwd = malloc(MAX_PATH_LENGTH * sizeof(char));
    if (pwd == NULL)
    {
        return NULL;
    }

    if (getcwd(pwd, MAX_PATH_LENGTH) == NULL)
    {
        return NULL;
    }

    return pwd;
}

int cd(const char *pathname){
    
    char *pwd = pwd_jsh();
    char *new_last_path = malloc(sizeof(char) * (strlen(pwd) + 1));
    strcpy(new_last_path, pwd);
    free(pwd);

    if(pathname == NULL){
        char *home = getenv("HOME");
        if(home == NULL || chdir(home) == -1){
            free(new_last_path);
            return 1;
        }
        free(last_path);
        last_path = new_last_path;
        return 0;
    }

    if(strcmp(pathname, "-") == 0){
        if(last_path != NULL){
            if(chdir(last_path) == -1){
                free(new_last_path);
                return 1;
            }
            free(last_path);
            last_path = new_last_path;
        }
        return 0;
    }
    
    if(chdir(pathname) == -1){
        free(new_last_path);
        return 1;
    }
    else{
        free(last_path);
        last_path = new_last_path;
    }
    
    return 0;
}

void redirection_1(struct argv_t * arg, int * last_return){
    int fd_file = open(arg->data[2], O_RDONLY);
    if (fd_file == -1){
        if (errno == ENOENT){
            fprintf(stderr, "%s: No such file or directory\n", arg->data[2]);
        }
        else{
            fprintf(stdout, "Error open file\n");
        }
    }
    else{
        int status;
        pid_t pids = fork();
        switch(pids){
            case 0:
                dup2(fd_file,STDIN_FILENO);
                close(fd_file);
                execlp(arg->data[0], arg->data[0], NULL);
                exit(1);
            default:
            wait(pids,&status,0);
            if (WIFEXITED(status)){
                *last_return = WEXITSTATUS(status);
            }
            else {
                *last_return = 1;
            }
            break;
        }
    }
}

void redirection_2(struct argv_t * arg, int * last_return){

}