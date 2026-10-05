#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/time.h>

#define MAX_COMMANDS 10
#define MAX_ARGS 20

void parse_command(char *input, char **args)
{
    int i = 0;
    char *token = strtok(input, " ");

    while (token != NULL && i < MAX_ARGS - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }

    args[i] = NULL;
}

int main()
{
    char input[500];
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    printf("===== Skill 23 =====\n");
    printf("Enter a pipeline:\n");
    printf("Example: ls | grep .c | wc -l\n\n");
    printf("myshell> ");

    if (fgets(input, sizeof(input), stdin) == NULL)
        return 1;

    input[strcspn(input, "\n")] = '\0';

    char *token = strtok(input, "|");

    while (token != NULL && command_count < MAX_COMMANDS)
    {
        while (*token == ' ')
            token++;

        commands[command_count++] = token;
        token = strtok(NULL, "|");
    }

    if (command_count == 0)
    {
        printf("No command entered\n");
        return 1;
    }

    int pipes[MAX_COMMANDS - 1][2];
    pid_t pids[MAX_COMMANDS];

    struct timeval start, end;

    gettimeofday(&start, NULL);

    for (int i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            return 1;
        }
    }

    for (int i = 0; i < command_count; i++)
    {
        pids[i] = fork();

        if (pids[i] < 0)
        {
            perror("fork");
            return 1;
        }

        if (pids[i] == 0)
        {
            if (i > 0)
                dup2(pipes[i - 1][0], STDIN_FILENO);

            if (i < command_count - 1)
                dup2(pipes[i][1], STDOUT_FILENO);

            for (int j = 0; j < command_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            char *args[MAX_ARGS];
            parse_command(commands[i], args);

            execvp(args[0], args);

            perror("execvp");
            exit(127);
        }
    }

    for (int i = 0; i < command_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    int failed = 0;

    for (int i = 0; i < command_count; i++)
    {
        int status;

        waitpid(pids[i], &status, 0);

        if (WIFEXITED(status))
        {
            int code = WEXITSTATUS(status);

            if (code != 0)
            {
                printf("Job %d failed with exit code %d\n", i + 1, code);
                failed = 1;
            }
        }
        else if (WIFSIGNALED(status))
        {
            printf("Job %d terminated by signal %d\n", i + 1, WTERMSIG(status));
            failed = 1;
        }
    }

    gettimeofday(&end, NULL);

    double execution_time =
        (end.tv_sec - start.tv_sec) +
        (end.tv_usec - start.tv_usec) / 1000000.0;

    printf("\nPipeline processes: %d\n", command_count);
    printf("Execution time: %.6f seconds\n", execution_time);

    if (failed)
        printf("Pipeline status: FAILED\n");
    else
        printf("Pipeline status: SUCCESS\n");

    return 0;
}
