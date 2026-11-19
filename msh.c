#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int parse_line(char *line, char **argv, int max);
int run_builtin(char **args, int argc, int *status, int *should_exit);
void reap_background(void);

int main(void)
{

    char *my_buffer = NULL;
    size_t collected_bytes_len = 0;
    int last_status = 0;
    signal(SIGINT, SIG_IGN);
    while (1)
    {
        reap_background();
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
        int background = 0;
        if (strcmp(my_args[arg_c - 1], "&") == 0)
        {
            background = 1;
            my_args[--arg_c] = NULL;
            if (arg_c == 0)
                continue;
        }
        int should_exit = 0;
        if (run_builtin(my_args, arg_c, &last_status, &should_exit))
        {
            if (should_exit)
                break;
            continue;
        }
        pid_t pid = fork();
        if (pid < 0)
        {
            perror("fork");
            last_status = 1;
            continue;
        }

        if (pid == 0)
        { /* child */
            if (!background)
                signal(SIGINT, SIG_DFL);
            execvp(my_args[0], my_args);
            perror(my_args[0]); /* only reached if exec failed */
            _exit(127);
        }
        if (background)
        {
            printf("[%d]\n", pid);
            last_status = 0;
            continue;
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
int run_builtin(char **args, int argc, int *status, int *should_exit)
{
    if (strcmp(args[0], "exit") == 0)
    {
        *should_exit = 1;
        return 1;
    }
    if (strcmp(args[0], "cd") == 0)
    {
        const char *dir = (argc > 1) ? args[1] : getenv("HOME");
        if (dir == NULL)
        {
            fprintf(stderr, "cd: HOME not set\n");
            *status = 1;
        }
        else if (chdir(dir) != 0)
        {
            perror("cd");
            *status = 1;
        }
        else
        {
            *status = 0;
        }
        return 1;
    }
    if (strcmp(args[0], "help") == 0)
    {
        printf("msh - a minimal shell\n"
               "Built-ins:\n"
               "  cd [dir]   change directory (no dir: go to $HOME)\n"
               "  help       show this message\n"
               "  exit       leave the shell\n"
               "Other commands run as child processes.\n"
               "  cmd &      run in the background\n"
               "  cmd > f    send output to file f\n");
        *status = 0;
        return 1;
    }
    return 0;
}

void reap_background(void){
    int st;
    pid_t pid;
    while ((pid = waitpid(-1, &st, WNOHANG)) > 0){
        if (WIFEXITED(st))
            printf("[%d] done, status %d\n", pid, WEXITSTATUS(st));
        else if (WIFSIGNALED(st))
            printf("[%d] killed by signal %d\n", pid, WTERMSIG(st));
    }
}