#include <stdlib.h>
#include <string.h>
#include <unistd.h>

char *lastPath;

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