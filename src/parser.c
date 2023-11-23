#include "parser.h"

char ** parse_line(char * line){

}

struct argv_t * split(char * line){

    struct argv_t * tab_data = malloc(sizeof(struct argv_t));
    size_t len_line = strlen(line);
    int nb_word = nb_words(line);

    char ** data ;
    int index = 0;

    data = malloc(sizeof(char *) * nb_word);

    if (data == NULL){
        fprintf(stderr, "error with malloc in split %lu\n", nb_word * sizeof(char *));
        exit(1);
    }

    char * word = strtok(line, " ");
    
    while (word != NULL){
        data[index] = word;
        word = strtok(NULL, " ");
        ++index;
    }

    tab_data->data = data;
    tab_data->len = nb_word;
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
