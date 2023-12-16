#include "parser.h"

struct argv_t * split(char * line){

    struct argv_t * tab_data = malloc(sizeof(struct argv_t));
    tab_data->esp = 0;
    int nb_word = nb_words(line);

    char ** data ;
    int index = 0;
    if (nb_word != 0){
        data = malloc(sizeof(char *) * (nb_word + 1));

        if (data == NULL){
            fprintf(stderr, "error with malloc in split\n");
            exit(1);
        }

        char * word = strtok(line, " ");
        
        while (word != NULL){
            if(strcmp(word, "&") == 0){
                ++tab_data->esp;
                --nb_word;
                word = strtok(NULL, " ");
            }
            else{
                data[index] = word;
                word = strtok(NULL, " ");
                ++index;
            }
        }

        data[nb_word] = NULL;
        tab_data->data = data;
        tab_data->len = nb_word;
    }
    else{
        tab_data->len = 0;
        tab_data->data = NULL;
    }

    // for(int i = 0; i< nb_word; ++i){
    //     if (data[i] != NULL){
    //         printf("%s\n", data[i]);
    //     }
    //     else
    //         printf("NULL\n");
    // }
    
    return tab_data;
}

int nb_words(char * line){
    size_t len_line = strlen(line);
    int nb_word = 0;
    int flag = 1;
    for(int i = 0; i < len_line; ++i){
        if (*(line + i) != ' ' && flag){
            ++nb_word;
            flag = 0;
        }
        if (*(line + i) == ' '){
            flag = 1;
        }
    }
    return nb_word;
}

/* check is a string contains one of the redirections characters */
int is_str_redirection(char * str){
    if (strcmp(str,"<") == 0 || strcmp(str,">") == 0 || strcmp(str,">|") == 0
    || strcmp(str,">>") == 0 || strcmp(str,"2>") == 0 || strcmp(str,"2>|") == 0
    || strcmp(str,"2>>") == 0 || strcmp(str,"|") == 0 || strcmp(str,"<(") == 0){
        return 1;
    }
    return 0;
}


/* check if a input line is a redirection if true return the index of the redirection string in arg->data */
int is_redirection(struct argv_t * arg){
    if (arg->len >= 3){ 
        for(int i = 1; i < arg->len; ++i){
            if (is_str_redirection(arg->data[i])){
                return i;
            }  
        }
    }
    return 0;
}

int which_redirection_str_is(char * str){
    if (strcmp(str,"<") == 0) 
        return 1;
    if (strcmp(str,">") == 0)
        return 2;
    if (strcmp(str,">|") == 0)
        return 3;
    if(strcmp(str,">>") == 0)
        return 4;
    if(strcmp(str,"2>") == 0)
        return 5;
    if(strcmp(str,"2>|") == 0)
        return 6;
    if(strcmp(str,"2>>") == 0)
        return 7;
    if(strcmp(str,"|") == 0)
        return 8;
    if(strcmp(str,"<(") == 0)
        return 9;
    else
        return 0;
}

/* associate redirection with number */
int which_redirection(struct argv_t * arg){
    if (arg->len >= 3){ 
        for(int i = 1; i < arg->len; ++i){
            int r = which_redirection_str_is(arg->data[i]);
            if (r > 0){
                return r;
            }
        }   
    }
    return 0;
}

/* return numbers of redirections in one input line */
int nb_direction(struct argv_t *arg){
    int nb = 0;
    for(int i = 1; i < arg->len; ++i){
        if (is_str_redirection(arg->data[i])){
            ++nb;
        }
    }
    return nb;
}

struct argv_t * data_cmd(struct argv_t * arg,int redir){
    struct argv_t * arg_cmd = malloc(sizeof(struct argv_t));
    char ** data_cmd = malloc(sizeof(char *) * (redir + 1));
    for(int i = 0; i < redir; ++i){
        data_cmd[i] = arg->data[i] ;
    }
    data_cmd[redir] = NULL;

    arg_cmd->data = data_cmd;
    arg_cmd->len = redir;

    return arg_cmd;
}