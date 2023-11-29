#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

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

    if(pathname == NULL){
        char *home = getenv("HOME");
        if(home == NULL || chdir(home) == -1){
            free(new_last_path);
            return 1;
        }
        //free(last_path);
        last_path = new_last_path;
        return 0;
    }

    if(strcmp(pathname, "-") == 0){
        if(last_path != NULL){
            if(chdir(last_path) == -1){
                free(new_last_path);
                return 1;
            }
            //free(last_path);
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
