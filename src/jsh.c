#include <stdio.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <ctype.h>
#include "parser.h"
#include "command.h"

#define SIZE_STR_INPUT 100

int main(int argc, char *argv[], char *envp[])
{

    signaux();

    // int fd_stdout_jsh = open("stdout_jsh", O_CREAT | O_TRUNC |O_RDWR, 0664);

     struct argv_t * arg;

    int last_command_return = 0;
    rl_outstream = stderr;

    while (1)
    {

        char *pwd = pwd_jsh();
        char *p = malloc(sizeof(char) * SIZE_STR_INPUT);
        if (p == NULL)
        {
            fprintf(stdout, "error malloc");
            exit(1);
        }
        *p = '\0';
        strcat(p, "\001\033[32m\002[");
        char *nb_jobs_tab = malloc(sizeof(char) * 2);
        nb_jobs_tab[0] = get_nb_jobs() + '0';
        nb_jobs_tab[1] = '\0';
        strcat(p, nb_jobs_tab);
        free(nb_jobs_tab);
        strcat(p, "]\001\033[36m\002");

        if(strlen(pwd) > 26){
            strcat(p, "...");
            strcat(p, pwd+strlen(pwd)-22);
        }
        else
        {
            strcat(p, pwd);
        }

        strcat(p, "\001\033[00m\002$ ");

        char *line = readline(p);
        free(p);
        free(pwd);
        if (line == NULL)
        {
            exit_jsh(last_command_return);
        }

        char *l = malloc(sizeof(char) * (strlen(line) + 1));
        strcpy(l, line);
        add_history(l);

        arg = split(line);
    
        if (arg->len != 0){
            int index_redirec = is_redirection(arg);
            if (strcmp(arg->data[0], "cd") == 0){
                if (arg->len == 1){
                    last_command_return = cd(NULL);
                    if (last_command_return == 1)
                    {
                        fprintf(stderr, "No such file or directory\n");
                    }
                }
                else
                {
                    last_command_return = cd(arg->data[1]);
                    if (last_command_return == 1)
                    {
                        fprintf(stderr, "bash: cd: %s: No such file or directory\n", arg->data[0]);
                    }
                }
            }

            else if (strcmp(arg->data[0], "pwd") == 0 && !index_redirec){
                pwd = pwd_jsh();
                fprintf(stdout, "%s\n", pwd);
                last_command_return = (pwd == NULL) ? 1 : 0;
                free(pwd);
            }

            else if (strcmp(arg->data[0], "exit") == 0){
                if (!get_nb_jobs()){
                    if (index_redirec){
                        free(arg->data);
                        free(arg);
                        free(line);
                        free(l);
                        exit_jsh(0);
                    }
                    if (arg->len == 1){
                        free(arg->data);
                        free(arg);
                        free(line);
                        free(l);
                        exit_jsh(last_command_return);
                    }
                    else if (arg->len == 2)
                    {
                        int val_exit = atoi(arg->data[1]);
                        free(arg->data);
                        free(arg);
                        free(line);
                        free(l);
                        exit_jsh(val_exit);
                    }
                    else
                    {
                        fprintf(stderr, "exit has at most two arguments\n");
                    }
                }
                else{
                    fprintf(stderr, "There are still jobs running\n");
                    last_command_return = 1;
                }
            }

            else if (strcmp(arg->data[0], "jobs") == 0)
            {
                remove_jobs();
                print_jobs();
            }

            else if (strcmp(arg->data[0], "kill") == 0 && strcmp(arg->data[1], "-l") != 0)
            {
                int sig = SIGTERM;
                if(arg->data[1][0] == '-')
                {
                    int sig_nb = strtol(arg->data[1] + 1, NULL, 10);
                    if(sig_nb < 65 && sig_nb > 0){
                        sig = atoi(arg->data[1] + 1);
                    }
                    else
                    {
                        fprintf(stderr, "-bash: kill: %s: invalid signal specification", arg->data[1] + 1);
                    }
                }
                else if(arg->data[1][0] == '%')
                {
                    if(kill_job(strtol(arg->data[1] + 1, NULL, 10), sig) == -1)
                    {
                        fprintf(stderr, "-bash: kill: %s: no such job\n", arg->data[1]);
                    }
                }
                else
                {
                    if(strtol(arg->data[1], NULL, 10) == 0 && strcmp(arg->data[1], "0") != 0)
                    {
                        fprintf(stderr, "-bash: kill: %s: arguments must be process or job IDs\n", arg->data[1]);
                    }
                    if(kill(strtol(arg->data[1], NULL, 10), sig) == -1)
                    {
                        fprintf(stderr, "-bash: kill: (%s) - No such process\n", arg->data[1]);
                    }

                }
                for(int i = 2 ; i < arg->len ; ++i)
                {
                    if(arg->data[i][0] == '%')
                    {
                        if(kill_job(strtol(arg->data[i] + 1, NULL, 10), sig) == -1)
                        {
                            fprintf(stderr, "-bash: kill: %s: no such job\n", arg->data[i]);
                        }
                    }
                    else if(kill(strtol(arg->data[i], NULL, 10), sig) == -1)
                    {
                        fprintf(stderr, "-bash: kill: (%s) - No such process\n", arg->data[i]);
                    }
                }
            }

            else if (strcmp(arg->data[0], "?") == 0)
            {
                fprintf(stdout, "%d\n", last_command_return);
                last_command_return = 0;
            }

            else
            {
                pid_t pids = fork();
                int status;

                switch (pids)
                {
                    case 0:
                    {
                        activate_sig();

                         int is_after_redir = 0;
                        int nb_redir = -1;
                        int first_redir = -1;
                        int fd_file = -1;
                        int redir_error = 0;

                        for(int i = 1 ; i < arg->len ; ++i)
                        {
                            activate_sig();

                            int is_after_redir = 0;
                            int nb_redir = -1;
                            int first_redir = -1;
                            int fd_file = -2;
                            int redir_error = 0;

                             for(int i = 1 ; i < arg->len ; ++i)
                            {
                                if(is_str_redirection(arg->data[i]))
                                {
                                    is_after_redir = 1;
                                    nb_redir = which_redirection_str_is(arg->data[i]);
                                    if(first_redir == -1)
                                        first_redir = i;
                                }
                                else if(is_after_redir == 1)
                                {
                                    if (nb_redir == 1){
                                        fd_file = redirection(&last_command_return, arg->data[i],0,O_RDONLY);
                                        dup2(fd_file, 0);
                                    }
                                    else if (nb_redir == 2 || nb_redir == 5){
                                        int option = O_WRONLY | O_EXCL | O_CREAT;
                                        fd_file = redirection(&last_command_return, arg->data[i],1,option);
                                        if(nb_redir == 2)
                                            dup2(fd_file, 1);
                                        else
                                            dup2(fd_file, 2);
                                    }
                                    else if (nb_redir == 3 || nb_redir == 6){
                                        int option = O_WRONLY | O_CREAT | O_TRUNC;
                                        fd_file = redirection(&last_command_return, arg->data[i],1,option);
                                        if(nb_redir == 3)
                                            dup2(fd_file, 1);
                                        else
                                            dup2(fd_file, 2);
                                    }
                                    else if (nb_redir == 4 || nb_redir == 7){
                                        int option = O_WRONLY | O_CREAT | O_APPEND;
                                        fd_file = redirection(&last_command_return, arg->data[i],1,option);
                                        if(nb_redir == 4)
                                            dup2(fd_file, 1);
                                        else
                                            dup2(fd_file, 2);
                                    }
                                    if(fd_file == -1)
                                        redir_error = 1;
                                }
                            }
                            else if(is_after_redir == 1)
                            {
                                int fd_file;
                                if (nb_redir == 1){
                                    fd_file = redirection(&last_command_return, arg->data[i],0,O_RDONLY);
                                    dup2(fd_file, 0);
                                }
                                else if (nb_redir == 2 || nb_redir == 5){
                                    int option = O_WRONLY | O_EXCL | O_CREAT;
                                    fd_file = redirection(&last_command_return, arg->data[i],1,option);
                                    if(nb_redir == 2)
                                        dup2(fd_file, 1);
                                    else
                                        dup2(fd_file, 2);
                                }
                                else if (nb_redir == 3 || nb_redir == 6){
                                    int option = O_WRONLY | O_CREAT | O_TRUNC;
                                    fd_file = redirection(&last_command_return, arg->data[i],1,option);
                                    if(nb_redir == 3)
                                        dup2(fd_file, 1);
                                    else
                                        dup2(fd_file, 2);
                                }
                                else if (nb_redir == 4 || nb_redir == 7){
                                    int option = O_WRONLY | O_CREAT | O_APPEND;
                                    fd_file = redirection(&last_command_return, arg->data[i],1,option);
                                    if(nb_redir == 4)
                                        dup2(fd_file, 1);
                                    else
                                        dup2(fd_file, 2);
                                }
                                if(fd_file == -1)
                                    redir_error = 1;
                            }
                        }

                        if(first_redir != -1)
                            arg->data[first_redir] = NULL;

                        if (redir_error == 0 && arg->data[0][0] == '.' || arg->data[0][0] == '/')
                        {
                            int r = execv(arg->data[0], arg->data);
                            if (r == -1)
                            {
                                if (arg->esp == 0) fprintf(stderr, "Unknown command\n");
                                else
                                    remove_jobs();
                            }
                        }
                        else if(redir_error == 0)
                        {
                            int r = execvp(arg->data[0], arg->data);
                            if (r == -1)
                            {
                                if (arg->esp == 0) fprintf(stderr, "Unknown command\n");
                                else
                                    remove_jobs();
                            }
                        }
                        free(arg->data);
                        free(arg);
                        free(line);
                        free(l);
                        exit(1);
                    }
                    default:
                    {
                        if (arg->esp == 0)
                            waitpid(pids, &status, 0);
                        else
                            add_job(pids, l);
                        if (WIFEXITED(status))
                        {
                            last_command_return = WEXITSTATUS(status);
                            // if (get_nb_jobs() > 0)
                            //     set_nb_jobs(get_nb_jobs() - 1);
                        }
                        else
                        {
                            last_command_return = 1;
                        }
                        break;
                    }
                }
            }
        free(arg->data);
        free(arg);
        free(line);
        free(l);
    }
    }
    return 0;
}