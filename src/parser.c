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

bool is_redirection(struct argv_t * arg){
    if (arg->len == 3){
        if (strcmp(arg->data[1],"<") == 0 || strcmp(arg->data[1],">") == 0 || strcmp(arg->data[1],">|") == 0
        || strcmp(arg->data[1],">>") == 0 || strcmp(arg->data[1],"2>") == 0 || strcmp(arg->data[1],"2>|") == 0
        || strcmp(arg->data[1],"2>>") == 0 || strcmp(arg->data[1],"|") == 0 || strcmp(arg->data[1],"<(") == 0){
            return true;
        }
        else{
            return false;
        }
    }
    else{
        return false;
    }
}