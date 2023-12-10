#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/wait.h>

#define EXIT_VAL 0
#define MAX_PATH_LENGTH 4096

char *last_path;

struct job {
    int id;
    char *state;
    char *name;
};

struct job *jobs[512];
int jobs_nb = 0;

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

// void jobs(){


// }
void add_job(int pid, char *name){

    jobs[jobs_nb] = malloc(sizeof(struct job));
    jobs[jobs_nb]->id = pid;
    jobs[jobs_nb]->state = "Running";
    jobs[jobs_nb]->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(jobs[jobs_nb]->name, name);
    ++jobs_nb;
}

void remove_jobs(){

    for(int i = 0 ; i < jobs_nb ; ++i){
        int status = 0;
        if(jobs[i] != NULL){
            if(waitpid(jobs[i]->id, &status, WNOHANG) > 0){
                if(WIFEXITED(status)){
                    free(jobs[i]->name);
                    free(jobs[i]);
                    jobs[i] = NULL;
                    if(i == jobs_nb-1)
                        --jobs_nb;
                }
            }
            
        }
    }
}

void remove_invalid_command(pid_t *pid){

    for(int i = 0 ; i < jobs_nb ; ++i){
        if(jobs[i] != NULL && jobs[i]->id == *pid){
            free(jobs[i]->name);
            free(jobs[i]);
            jobs[i] = NULL;
            if(i == jobs_nb-1)
                --jobs_nb;
        }
    }
}

void print_jobs(){

    for(int i = 0 ; i < jobs_nb ; ++i){
        if(jobs[i] != NULL)
            fprintf(stdout, "[%d] %d  %s  %s\n", i+1, jobs[i]->id, jobs[i]->state, jobs[i]->name);
    }
}
