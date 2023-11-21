#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <libgen.h>
#include <sys/param.h>
#include <unistd.h>

char *lastPath;

int cd(char *pathname){

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