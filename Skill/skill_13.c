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
    int fd;

    char *redirect = strchr(input, '<');

    if (redirect != NULL)
    {
        *redirect = '\0';
        redirect++;

        while (*redirect == ' ')
            redirect++;

        char *filename = strtok(redirect, " ");

        if (filename == NULL)
        {
            printf("Missing input file\n");
            return;
        }

        fd = open(filename, O_RDONLY);

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
        if (redirect != NULL)
        {
            dup2(fd, STDIN_FILENO);
            close(fd);
        }

        execvp(args[0], args);

        perror("execvp");
        exit(1);
    }

    waitpid(pid, NULL, 0);

    if (redirect != NULL)
        close(fd);
}

int main()
{
    char input[MAX_INPUT];

    printf("===== Skill 13 =====\n");
    printf("Enter command: ");

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    execute_command(input);

    return 0;
}
