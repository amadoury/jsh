#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "parser.h"
 
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

pid_t job_to_remove;

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

/* for redirection */
void redirection(struct argv_t * arg, int * last_return, int redir, int mode, int option, int nb_redir){
    int fd_file ;
    if (mode){
        fd_file = open(arg->data[redir + 1], option, 0664);
    }
    else {
        fd_file = open(arg->data[redir + 1], option);
    }

    if (fd_file == -1){
        if (errno == ENOENT){
            fprintf(stderr, "%s: No Such File or Directory\n", arg->data[redir + 1]);
        }
        if (errno == EEXIST){
            fprintf(stderr, "%s: File already exist\n", arg->data[redir + 1]);
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
                if (nb_redir == 1){
                    dup2(fd_file,STDIN_FILENO);
                    close(fd_file);
                }
                if (nb_redir == 2 || nb_redir == 3 || nb_redir == 4){
                    dup2(fd_file, STDOUT_FILENO);
                    close(fd_file); 
                }
                if (nb_redir == 5 || nb_redir == 6 || nb_redir == 7){
                    dup2(fd_file, STDERR_FILENO);
                    close(fd_file);
                }   

                struct argv_t * arg_cmd = data_cmd(arg,redir);
                int r = execvp(arg_cmd->data[0], arg_cmd->data);
                if (r == -1){
                    fprintf(stdout, "Unknown command\n");
                }
                exit(1);
            default:
            waitpid(pids,&status,0);
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
                if(WIFEXITED(status) || WIFSIGNALED(status)){
                    jobs[i]->state = "Done   ";
                }
                if(WIFSTOPPED(status)){
                    jobs[i]->state = "Stopped";
                }
            }
            
        }
    }
}

void add_job_to_remove(pid_t pid){
    job_to_remove = pid;
}

void remove_invalid_command(){
    printf("removing %d\n", job_to_remove);
    for(int i = 0 ; i < jobs_nb ; ++i){
        if(jobs[i] != NULL && jobs[i]->id == job_to_remove){
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
        if(jobs[i] != NULL){
            fprintf(stdout, "[%d] %d  %s  %s\n", i+1, jobs[i]->id, jobs[i]->state, jobs[i]->name);
            if(strcmp(jobs[i]->state, "Done   ") == 0){
                free(jobs[i]->name);
                free(jobs[i]);
                jobs[i] = NULL;
                if(i == jobs_nb-1){
                    int j = i;
                    while(j >= 0 && (jobs[i] == NULL || (strcmp(jobs[i]->state, "Done   ") == 0))){
                        --jobs_nb;
                        --j;
                    }
                }
            }
        }
    }
}
