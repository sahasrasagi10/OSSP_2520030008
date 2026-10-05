#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>

#define MAX_COMMAND 200

pid_t foreground_pid = -1;

void handle_sigint(int sig)
{
    if (foreground_pid > 0)
    {
        kill(foreground_pid, SIGINT);
    }
    else
    {
        printf("\nmyshell> ");
        fflush(stdout);
    }
}

void execute_command(char *input)
{
    char *args[50];
    int count = 0;

    char temp[MAX_COMMAND];
    strcpy(temp, input);

    char *token = strtok(temp, " ");

    while (token != NULL && count < 49)
    {
        args[count++] = token;
        token = strtok(NULL, " ");
    }

    args[count] = NULL;

    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        signal(SIGINT, SIG_DFL);

        execvp(args[0], args);

        perror("execvp");
        exit(1);
    }

    foreground_pid = pid;

    waitpid(pid, NULL, 0);

    foreground_pid = -1;
}

int main()
{
    char input[MAX_COMMAND];

    signal(SIGINT, handle_sigint);

    printf("===== Skill 19 =====\n");

    while (1)
    {
        printf("\nmyshell> ");

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
        {
            break;
        }

        if (strlen(input) == 0)
        {
            continue;
        }

        execute_command(input);
    }

    return 0;
}
