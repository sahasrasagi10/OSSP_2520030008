#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_INPUT 200
#define MAX_ARGS 20

void execute_command(char *input)
{
    char *args[MAX_ARGS];
    int count = 0;
    int fd = -1;

    char *redirect = strchr(input, '>');

    if (redirect != NULL)
    {
        *redirect = '\0';
        redirect++;

        while (*redirect == ' ')
            redirect++;

        char *filename = strtok(redirect, " ");

        if (filename == NULL)
        {
            printf("Missing output file\n");
            return;
        }

        fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);

        if (fd == -1)
        {
            perror("open");
            return;
        }
    }

    char *token = strtok(input, " ");

    while (token != NULL && count < MAX_ARGS - 1)
    {
        args[count++] = token;
        token = strtok(NULL, " ");
    }

    args[count] = NULL;

    pid_t pid = fork();

    if (pid == -1)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        if (fd != -1)
        {
            dup2(fd, STDOUT_FILENO);
            close(fd);
        }

        execvp(args[0], args);

        perror("execvp");
        exit(1);
    }

    waitpid(pid, NULL, 0);

    if (fd != -1)
        close(fd);
}

int main()
{
    char input[MAX_INPUT];

    printf("===== Skill 14 =====\n");
    printf("Enter command: ");

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    execute_command(input);

    return 0;
}
