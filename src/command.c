#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>

#define EXIT_VAL 0
#define MAX_PATH_LENGTH 1024

char *lastPath;

void exit_jsh(int val)
{
    exit(val);
}

int cd(const char *pathname){
    if(pathname == NULL){
        char *home = getenv("HOME");
        fprintf(stderr,"%s\n",home);
        if(home == NULL) return 1;
        if(chdir(home) == -1) return 1;
        return 0;
    }
    if(strcmp(pathname, "-") == 0){
        if(lastPath != NULL){
            if(chdir(lastPath) == -1) return 1;
        }
        return 0;
    }
    if(chdir(pathname) == -1) return 1;
    return 0;
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