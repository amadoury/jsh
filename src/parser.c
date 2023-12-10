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

int is_redirection(struct argv_t * arg){
    if (arg->len >= 3){ 
        for(int i = 1; i < arg->len; ++i){
            if (strcmp(arg->data[i],"<") == 0 || strcmp(arg->data[i],">") == 0 || strcmp(arg->data[i],">|") == 0
            || strcmp(arg->data[i],">>") == 0 || strcmp(arg->data[i],"2>") == 0 || strcmp(arg->data[i],"2>|") == 0
            || strcmp(arg->data[i],"2>>") == 0 || strcmp(arg->data[i],"|") == 0 || strcmp(arg->data[i],"<(") == 0){
                return i;
            }
        }   
    }
    return 0;
}

int which_redirection(struct argv_t * arg){
    if (arg->len >= 3){ 
        for(int i = 1; i < arg->len; ++i){
            if (strcmp(arg->data[i],"<") == 0) 
                return 1;
            if (strcmp(arg->data[i],">") == 0)
                return 2;
            if (strcmp(arg->data[i],">|") == 0)
                return 3;
            if(strcmp(arg->data[i],">>") == 0)
                return 4;
            if(strcmp(arg->data[i],"2>") == 0)
                return 5;
            if(strcmp(arg->data[i],"2>|") == 0)
                return 6;
            if(strcmp(arg->data[i],"2>>") == 0)
                return 7;
            if(strcmp(arg->data[i],"|") == 0)
                return 8;
            if(strcmp(arg->data[i],"<(") == 0)
                return 9;
        }   
    }
    return 0;
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