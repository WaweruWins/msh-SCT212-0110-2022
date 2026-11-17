#include <stdio.h>
#include <stdlib.h>

int main(void){

    char *my_buffer = NULL;
    size_t collected_bytes_len = 0;
    ssize_t get_line_result;

    while (1){
        printf("msh> ");
        fflush(stdout);
        get_line_result = getline(&my_buffer, &collected_bytes_len, stdin);
        if (get_line_result == -1) {
            printf("\n");
            break;
        }
        printf("Input Line: %s", my_buffer);
}
    free(my_buffer);

    return 0;
}