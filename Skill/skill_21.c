#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <errno.h>
#include <time.h>

#define MAX_COMMAND 200
#define MAX_ARGS 50

void log_error(const char *message)
{
    FILE *file = fopen("shell_errors.log", "a");

    if (file == NULL)
        return;

    time_t now = time(NULL);

    fprintf(file, "[%s] %s\n", ctime(&now), message);

    fclose(file);
}

void execute_command(char *input)
{
    char *args[MAX_ARGS];
    int count = 0;

    char temp[MAX_COMMAND];
    strcpy(temp, input);

    char *token = strtok(temp, " ");

    while (token != NULL && count < MAX_ARGS - 1)
    {
        args[count++] = token;
        token = strtok(NULL, " ");
    }

    args[count] = NULL;

    if (count == 0)
        return;

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        log_error("fork failed");
        return;
    }

    if (pid == 0)
    {
        execvp(args[0], args);

        perror("execvp");
        exit(127);
    }

    int status;

    if (waitpid(pid, &status, 0) < 0)
    {
        perror("waitpid");
        log_error("waitpid failed");
        return;
    }

    if (WIFEXITED(status))
    {
        int code = WEXITSTATUS(status);

        if (code != 0)
        {
            printf("Command failed with exit code %d\n", code);
            log_error("Command execution failed");
        }
    }
    else if (WIFSIGNALED(status))
    {
        printf("Command terminated by signal %d\n", WTERMSIG(status));
        log_error("Command terminated by signal");
    }
}

int main()
{
    char input[MAX_COMMAND];

    printf("===== Skill 21 =====\n");

    while (1)
    {
        printf("\nmyshell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strlen(input) == 0)
            continue;

        if (strcmp(input, "exit") == 0)
            break;

        if (strcmp(input, "help") == 0)
        {
            printf("Commands: help, exit, pwd, ls, date, invalid commands\n");
            continue;
        }

        execute_command(input);
    }

    return 0;
}
