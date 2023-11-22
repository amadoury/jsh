#ifndef PWD_H
#define PWD_H

#include "jsh.h"

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

#endif