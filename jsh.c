#include "cd.h"
#include "exit.h"
#include "jsh.h"
#include "pwd.h"
#include "prompt.h"

int main(int argc, char *argv[])
{

    if (argc > 2)
    {
        printf("Trop d'arguments\n");
        exitJSH(1);
    }

    if (argc == 2)
    {
        if (strcmp(argv[1], "pwd") == 0)
        {
            char *pwd = pwdJSH();
            printf("%s\n", pwd);
            free(pwd);
        }
        else if (strcmp(argv[1], "exit") == 0)
        {
            exitJSH(EXIT_VAL);
        }
        else
        {
            printf("Commande inconnue\n");
            exitJSH(1);
        }
    }

    return 0;
}
