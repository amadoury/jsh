#include "build.h"

char *build_prompt() {
    char *prompt = malloc(sizeof(char) * MAX_PROMPT_LENGTH);
    if (!prompt) {
        fprintf(stderr, "Error allocating memory\n");
        exit(1);
    }

    char *pwd = pwd_jsh();
    int nb_jobs = get_nb_jobs();

    snprintf(prompt, MAX_PROMPT_LENGTH, "%s[%d]%s%s%s%s$ ",
             PROMPT_GREEN, nb_jobs, PROMPT_CYAN,
             (strlen(pwd) > 26) ? "..." : "",
             (strlen(pwd) > 26) ? pwd + strlen(pwd) - 22 : pwd,
             PROMPT_RESET);

    free(pwd);
    return prompt;
}

void build_cd(struct argv_t *arg) {
    if (arg->len == 1) {
        last_command_return = cd(NULL);
        if (last_command_return == 1) {
            fprintf(stderr, "No such file or directory\n");
        }
    } else {
        last_command_return = cd(arg->data[1]);
        if (last_command_return == 1) {
            fprintf(stderr, "bash: cd: %s: No such file or directory\n", arg->data[0]);
        }
    }
}

void build_pwd() {
    pwd = pwd_jsh();
    fprintf(stdout, "%s\n", pwd);
    last_command_return = (pwd == NULL) ? 1 : 0;
    free(pwd);
}

void build_exit(struct argv_t *arg) {
    if (!get_nb_jobs()) {
        if (index_redirec) {
            free(arg->data);
            free(arg);
            free(line);
            free(l);
            exit_jsh(0);
        }
        if (arg->len == 1) {
            free(arg->data);
            free(arg);
            free(line);
            free(l);
            exit_jsh(last_command_return);
        } else if (arg->len == 2) {
            int val_exit = atoi(arg->data[1]);
            free(arg->data);
            free(arg);
            free(line);
            free(l);
            exit_jsh(val_exit);
        } else {
            fprintf(stderr, "exit has at most two arguments\n");
        }
    } else {
        fprintf(stderr, "There are still jobs running\n");
        last_command_return = 1;
    }
}

void build_jobs() {
    remove_jobs(0, -1);
    print_jobs();
}

void build_kill(struct argv_t *arg) {
    int sig = SIGTERM;
    if (arg->data[1][0] == '-') {
        int sig_nb = strtol(arg->data[1] + 1, NULL, 10);
        if (sig_nb < 65 && sig_nb > 0) {
            sig = atoi(arg->data[1] + 1);
        } else {
            fprintf(stderr, "-bash: kill: %s: invalid signal specification", arg->data[1] + 1);
        }
    } else if (arg->data[1][0] == '%') {
        if (kill_job(strtol(arg->data[1] + 1, NULL, 10), sig) == -1) {
            fprintf(stderr, "-bash: kill: %s: no such job\n", arg->data[1]);
        }
    } else {
        if (strtol(arg->data[1], NULL, 10) == 0 && strcmp(arg->data[1], "0") != 0) {
            fprintf(stderr, "-bash: kill: %s: arguments must be process or job IDs\n", arg->data[1]);
        }
        if (kill(strtol(arg->data[1], NULL, 10), sig) == -1) {
            fprintf(stderr, "-bash: kill: (%s) - No such process\n", arg->data[1]);
        }
    }
    for (int i = 2; i < arg->len; ++i) {
        if (arg->data[i][0] == '%') {
            if (kill_job(strtol(arg->data[i] + 1, NULL, 10), sig) == -1) {
                fprintf(stderr, "-bash: kill: %s: no such job\n", arg->data[i]);
            }
        } else if (kill(strtol(arg->data[i], NULL, 10), sig) == -1) {
            fprintf(stderr, "-bash: kill: (%s) - No such process\n", arg->data[i]);
        }
    }
}

void build_interogation() {
    fprintf(stdout, "%d\n", last_command_return);
    last_command_return = 0;
}

void build_external(struct argv_t *arg) {
    pid_t pids = fork();
    int status = 0;

    switch (pids) {
    case 0: {
        activate_sig();

        int is_after_redir = 0;
        int nb_redir = -1;
        int first_redir = -1;
        int fd_file = -2;
        int redir_error = 0;

        for (int i = 1; i < arg->len; ++i) {
            if (is_str_redirection(arg->data[i])) {
                is_after_redir = 1;
                nb_redir = which_redirection_str_is(arg->data[i]);
                if (first_redir == -1)
                    first_redir = i;
            } else if (is_after_redir == 1) {
                if (nb_redir == 1) {
                    fd_file = redirection(&last_command_return, arg->data[i], 0, O_RDONLY);
                    dup2(fd_file, 0);
                } else if (nb_redir == 2 || nb_redir == 5) {
                    int option = O_WRONLY | O_EXCL | O_CREAT;
                    fd_file = redirection(&last_command_return, arg->data[i], 1, option);
                    if (nb_redir == 2)
                        dup2(fd_file, 1);
                    else
                        dup2(fd_file, 2);
                } else if (nb_redir == 3 || nb_redir == 6) {
                    int option = O_WRONLY | O_CREAT | O_TRUNC;
                    fd_file = redirection(&last_command_return, arg->data[i], 1, option);
                    if (nb_redir == 3)
                        dup2(fd_file, 1);
                    else
                        dup2(fd_file, 2);
                } else if (nb_redir == 4 || nb_redir == 7) {
                    int option = O_WRONLY | O_CREAT | O_APPEND;
                    fd_file = redirection(&last_command_return, arg->data[i], 1, option);
                    if (nb_redir == 4)
                        dup2(fd_file, 1);
                    else
                        dup2(fd_file, 2);
                }
                if (fd_file == -1)
                    redir_error = 1;
            }
        }

        if (first_redir != -1)
            arg->data[first_redir] = NULL;

        if (redir_error == 0 && (arg->data[0][0] == '.' || arg->data[0][0] == '/')) {
            int r = execv(arg->data[0], arg->data);
            if (r == -1) {
                if (arg->esp == 0)
                    fprintf(stderr, "Unknown command\n");
                else
                    remove_jobs(0, -1);
            }
        } else if (redir_error == 0) {
            int r = execvp(arg->data[0], arg->data);
            if (r == -1) {
                if (arg->esp == 0)
                    fprintf(stderr, "Unknown command\n");
                else
                    remove_jobs(0, -1);
            }
        }
        free(arg->data);
        free(arg);
        free(line);
        free(l);
        exit(1);
    }
    default: {
        add_job(pids, l);
        if (arg->esp == 0) {
            tcsetpgrp(STDIN_FILENO, pids);
            tcsetpgrp(STDOUT_FILENO, pids);
            if (waitpid(pids, &status, WUNTRACED) != -1) {
                if (!WIFSTOPPED(status)) {
                    remove_jobs(0, 1);
                } else {
                    turn_to_background(pids);
                }
                tcsetpgrp(STDIN_FILENO, getpid());
                tcsetpgrp(STDOUT_FILENO, getpid());
            }
        }

        if (WIFEXITED(status)) {
            last_command_return = WEXITSTATUS(status);
        } else {
            last_command_return = 1;
        }
        break;
    }
    }
}

void build_clean(struct argv_t *arg) {
    free(arg->data);
    free(arg);
    free(line);
    free(l);
}

void execute_command(char *cmd) {
    const int MAX_ARGS = 4096;
    char *args[MAX_ARGS];
    char *token = strtok(cmd, " ");
    int i = 0;
    while (token != NULL && i < MAX_ARGS) {
        args[i++] = token;
        token = strtok(NULL, " ");
    }
    args[i] = NULL;

    execvp(args[0], args);
    
    perror("execvp");
    exit(EXIT_FAILURE);
}

void build_pipe(char **cmds, int n_pipes) {
    int pipefds[2 * n_pipes];

    for (int i = 0; i < n_pipes; i++) {
        if (pipe(pipefds + i*2) < 0) {
            perror("Erreur de création de pipe");
            exit(EXIT_FAILURE);
        }
    }

    int pid;
    for (int i = 0; i < n_pipes + 1; i++) {
        pid = fork();
        if (pid == 0) {
            if (i != 0) {
                dup2(pipefds[(i - 1) * 2], 0);
            }
            if (i != n_pipes) {
                dup2(pipefds[i * 2 + 1], 1);
            }
            for (int j = 0; j < 2 * n_pipes; j++) {
                close(pipefds[j]);
            }
            execute_command(cmds[i]);
        } else if (pid < 0) {
            perror("Erreur fork");
            exit(EXIT_FAILURE);
        }
    }

    for (int i = 0; i < 2 * n_pipes; i++) {
        close(pipefds[i]);
    }

    for (int i = 0; i < n_pipes + 1; i++) {
        wait(NULL);
    }
}
