#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <signal.h>
#include "parser.h"

#define EXIT_VAL 0
#define MAX_PATH_LENGTH 4096
#define BLOCK_SIZE 512

char *last_path;

struct job
{
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

int cd(const char *pathname)
{

    char *pwd = pwd_jsh();
    char *new_last_path = malloc(sizeof(char) * (strlen(pwd) + 1));
    strcpy(new_last_path, pwd);
    free(pwd);

    if (pathname == NULL)
    {
        char *home = getenv("HOME");
        if (home == NULL || chdir(home) == -1)
        {
            free(new_last_path);
            return 1;
        }
        free(last_path);
        last_path = new_last_path;
        return 0;
    }

    if (strcmp(pathname, "-") == 0)
    {
        if (last_path != NULL)
        {
            if (chdir(last_path) == -1)
            {
                free(new_last_path);
                return 1;
            }
            free(last_path);
            last_path = new_last_path;
        }
        return 0;
    }

    if (chdir(pathname) == -1)
    {
        free(new_last_path);
        return 1;
    }
    else
    {
        free(last_path);
        last_path = new_last_path;
    }

    return 0;
}

/* for redirection */
int redirection(int * last_return, char * file, int mode, int option){
    int fd_file ;
    if (mode){
        fd_file = open(file, option, 0664);
    }
    else {
        fd_file = open(file, option);
    }

    if (fd_file == -1){
        if (errno == ENOENT){
            fprintf(stderr, "%s: No Such File or Directory\n", file);
            *last_return = 1;
            return -1;
        }
        if (errno == EEXIST){
            fprintf(stderr, "%s: File already exist\n", file);
            *last_return = 1;
            return -1;
        }
        else
        {
            fprintf(stdout, "Error open file\n");
            *last_return = 1;
            return -1;
        }
    }
    
    return fd_file;
}

void add_job(int pid, char *name){
    //setpgid(pid, 0);
    jobs[jobs_nb] = malloc(sizeof(struct job));
    jobs[jobs_nb]->id = pid;
    jobs[jobs_nb]->state = "Running";
    jobs[jobs_nb]->name = malloc(sizeof(char) * (strlen(name) + 1));
    strcpy(jobs[jobs_nb]->name, name);
    *(jobs[jobs_nb]->name + strlen(name) - 1) = '\0';
    fprintf(stderr, "[%d] %d  %s  %s\n", jobs_nb, jobs[jobs_nb]->id, jobs[jobs_nb]->state, jobs[jobs_nb]->name);
    ++jobs_nb;
}

void remove_jobs()
{

    for (int i = 0; i < jobs_nb; ++i)
    {
        int status = 0;
        if (jobs[i] != NULL)
        {
            if (waitpid(jobs[i]->id, &status, WNOHANG) > 0)
            {
                if (WIFEXITED(status) || WIFSIGNALED(status))
                {
                    jobs[i]->state = "Done   ";
                    fprintf(stderr, "[%d] %d  %s  %s\n", jobs_nb, jobs[jobs_nb]->id, jobs[jobs_nb]->state, jobs[jobs_nb]->name);
                }
                if (WIFSTOPPED(status))
                {
                    jobs[i]->state = "Stopped";
                }
            }
        }
    }
}

void add_job_to_remove(pid_t pid)
{
    job_to_remove = pid;
}

void remove_invalid_command()
{
    printf("removing %d\n", job_to_remove);
    for (int i = 0; i < jobs_nb; ++i)
    {
        if (jobs[i] != NULL && jobs[i]->id == job_to_remove)
        {
            free(jobs[i]->name);
            free(jobs[i]);
            jobs[i] = NULL;
            if (i == jobs_nb - 1)
                --jobs_nb;
        }
    }
}

void print_jobs()
{

    for (int i = 0; i < jobs_nb; ++i)
    {
        if (jobs[i] != NULL)
        {
            fprintf(stdout, "[%d] %d  %s  %s\n", i + 1, jobs[i]->id, jobs[i]->state, jobs[i]->name);
            if (strcmp(jobs[i]->state, "Done   ") == 0)
            {
                free(jobs[i]->name);
                free(jobs[i]);
                jobs[i] = NULL;
                if (i == jobs_nb - 1)
                {
                    int j = i;
                    while (j >= 0 && (jobs[i] == NULL || (strcmp(jobs[i]->state, "Done   ") == 0)))
                    {
                        --jobs_nb;
                        --j;
                    }
                }
            }
        }
    }
}

int get_nb_jobs()
{
    return jobs_nb;
}

int set_nb_jobs(int nb){
    jobs_nb = nb;
}

int kill_job(int n, int sig)
{
    if(jobs[n - 1] == NULL) return -1;
    if(kill(jobs[n - 1]->id, sig) == -1)
        return -1;
    else
    {
        return 0;
    }
}

void signaux()
{
    struct sigaction actINTbash, actTERMbash, actTSTPbash;

    memset(&actINTbash, 0, sizeof(actINTbash));
    memset(&actTERMbash, 0, sizeof(actTERMbash));
    memset(&actTSTPbash, 0, sizeof(actTSTPbash));

    actINTbash.sa_handler = SIG_IGN;
    actTERMbash.sa_handler = SIG_IGN;
    actTSTPbash.sa_handler = SIG_IGN;

    sigaction(SIGINT, &actINTbash, NULL);
    sigaction(SIGTERM, &actTERMbash, NULL);
    sigaction(SIGTSTP, &actTSTPbash, NULL);
}

void sig_job(int sig)
{
    fprintf(stderr, "sig %d arrived\n", sig);
    for (int i = 0; i < jobs_nb; ++i)
    {
        if (jobs[i] != NULL && jobs[i]->id == getpid())
        {
            if(sig == 9)
                jobs[i]->state = "Killed ";
        }
    }
}

void activate_sig()
{
    struct sigaction actINTbash, actTERMbash, actTSTPbash, actKILLbash;

    memset(&actINTbash, 0, sizeof(actINTbash));
    memset(&actTERMbash, 0, sizeof(actTERMbash));
    memset(&actTSTPbash, 0, sizeof(actTSTPbash));
    memset(&actKILLbash, 0, sizeof(actKILLbash));

    actINTbash.sa_handler = SIG_DFL;
    actTERMbash.sa_handler = SIG_DFL;
    actTSTPbash.sa_handler = sig_job;
    actKILLbash.sa_handler = sig_job;

    sigaction(SIGINT, &actINTbash, NULL);
    sigaction(SIGTERM, &actTERMbash, NULL);
    sigaction(SIGTSTP, &actTSTPbash, NULL);
    sigaction(SIGKILL, &actKILLbash, NULL);

}