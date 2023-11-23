#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "command.h"

#define EXIT_VAL 0
#define MAX_PATH_LENGTH 1024

void exit_jsh(int val)
{
    exit(val);
}

int cd(const char *pathname){

    if(*pathname == '\0'){
        char *home = getenv("HOME");
        if(home == NULL) return 1;
        if(chdir(home) == -1) return 1;
        return 0;
    }
    if(strcmp(pathname, "-") == 0){
        if(last_path != NULL){
            if(chdir(last_path) == -1) return 1;
        }
        return 0;
    }
    if(chdir(pathname) == -1) return 1;
    free(last_path);
    last_path = malloc(sizeof(char) * (strlen(pathname + 1)));
    strcpy(last_path, pathname);
    return 0;
}

char *pwd_jsh()
{
    char *pwd = (char *)malloc(MAX_PATH_LENGTH * sizeof(char));
    if (pwd == NULL)
    {
        perror("Erreur lors de l'allocation de mémoire");
        exit_jsh(1);
    }

    char *cwd = (char *)malloc(MAX_PATH_LENGTH * sizeof(char));
    if (cwd == NULL)
    {
        perror("Erreur lors de l'allocation de mémoire");
        exit_jsh(1);
    }

    if (getcwd(cwd, MAX_PATH_LENGTH) == NULL)
    {
        perror("Erreur lors de la récupération du répertoire de travail actuel");
        exit_jsh(1);
    }

    strcpy(pwd, cwd);
    free(cwd);

    return pwd;
}