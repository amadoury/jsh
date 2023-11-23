#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define EXIT_VAL 0
#define MAX_PATH_LENGTH 1024

char *lastPath;

void exitJSH(int val)
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
        if(lastPath != NULL){
            if(chdir(lastPath) == -1) return 1;
        }
        return 0;
    }
    if(chdir(pathname) == -1) return 1;
    return 0;
}

char *pwdJSH()
{
    char *pwd = (char *)malloc(MAX_PATH_LENGTH * sizeof(char));
    if (pwd == NULL)
    {
        perror("Erreur lors de l'allocation de mémoire");
        exitJSH(1);
    }

    char *cwd = (char *)malloc(MAX_PATH_LENGTH * sizeof(char));
    if (cwd == NULL)
    {
        perror("Erreur lors de l'allocation de mémoire");
        exitJSH(1);
    }

    if (getcwd(cwd, MAX_PATH_LENGTH) == NULL)
    {
        perror("Erreur lors de la récupération du répertoire de travail actuel");
        exitJSH(1);
    }

    strcpy(pwd, cwd);
    free(cwd);

    return pwd;
}