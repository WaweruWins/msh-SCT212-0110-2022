#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int parse_line(char *line, char **argv, int max);


int main(void){

    char *my_buffer = NULL;
    size_t collected_bytes_len = 0;
    /*loop to print the shell flushing 
    prompt and collect the whole line*/
    while (1){
        printf("msh> ");
        fflush(stdout);
        //collecting all what was typed in the line
        if ((getline(&my_buffer, &collected_bytes_len, stdin)) == -1) {
            printf("\n");
            break;}
        char *my_args[64];

        int arg_c = parse_line(my_buffer, my_args, 64);
        if (arg_c != 0){
            for (int i = 0; i < arg_c; i++){
                printf("argv[%d]=[%s]\n", i, my_args[i]);
            }
        }
    }
    free(my_buffer);

    return 0;
}

int parse_line(char *line, char **argv, int max){

    char *first_word = strtok(line, " \t\n");
    int count = 0;
    while (first_word != NULL && count < max - 1){
        argv[count] = first_word;
        count++;
        first_word = strtok(NULL, " \t\n");

    }
    argv[count] = NULL;
    return count;

    }
