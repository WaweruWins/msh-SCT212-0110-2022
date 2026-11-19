#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int parse_line(char *line, char **argv, int max);

int main(void)
{

    char *my_buffer = NULL;
    size_t collected_bytes_len = 0;
    int last_status = 0;
    /*loop to print the shell flushing
    prompt and collect the whole line*/
    while (1)
    {
        printf("msh[%d]> ", last_status);
        fflush(stdout);
        // collecting all what was typed in the line
        if ((getline(&my_buffer, &collected_bytes_len, stdin)) == -1)
        {
            printf("\n");
            break;
        }
        char *my_args[64];

        int arg_c = parse_line(my_buffer, my_args, 64);
        if (arg_c == 0)
            continue;

        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            last_status = 1;
            continue;
        }

        if (pid == 0)
        { /* child */
            execvp(my_args[0], my_args);
            perror(my_args[0]); /* only reached if exec failed */
            _exit(127);
        }

        int status; /* parent */
        if (waitpid(pid, &status, 0) < 0)
        {
            perror("waitpid");
            last_status = 1;
            continue;
        }
        if (WIFEXITED(status))
            last_status = WEXITSTATUS(status);
        else if (WIFSIGNALED(status))
        {
            last_status = 128 + WTERMSIG(status);
            printf("\n");
        }
    }
    free(my_buffer);

    return 0;
}

int parse_line(char *line, char **argv, int max)
{

    char *first_word = strtok(line, " \t\n");
    int count = 0;
    while (first_word != NULL && count < max - 1)
    {
        argv[count] = first_word;
        count++;
        first_word = strtok(NULL, " \t\n");
    }
    argv[count] = NULL;
    return count;
}
