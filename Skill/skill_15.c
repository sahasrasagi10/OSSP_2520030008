#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>

#define MAX_INPUT 500
#define MAX_COMMANDS 10
#define MAX_ARGS 30

void parse_args(char *command, char **args)
{
    int i = 0;
    char *token = strtok(command, " ");

    while (token != NULL && i < MAX_ARGS - 1)
    {
        args[i++] = token;
        token = strtok(NULL, " ");
    }

    args[i] = NULL;
}

void execute_pipeline(char *input)
{
    char *commands[MAX_COMMANDS];
    int command_count = 0;

    char *token = strtok(input, "|");

    while (token != NULL && command_count < MAX_COMMANDS)
    {
        commands[command_count++] = token;
        token = strtok(NULL, "|");
    }

    int pipes[MAX_COMMANDS - 1][2];
    pid_t pids[MAX_COMMANDS];

    for (int i = 0; i < command_count - 1; i++)
    {
        if (pipe(pipes[i]) == -1)
        {
            perror("pipe");
            return;
        }
    }

    for (int i = 0; i < command_count; i++)
    {
        pids[i] = fork();

        if (pids[i] == -1)
        {
            perror("fork");
            return;
        }

        if (pids[i] == 0)
        {
            char *input_file = NULL;
            char *output_file = NULL;

            char *in_redirect = strchr(commands[i], '<');
            char *out_redirect = strchr(commands[i], '>');

            if (in_redirect != NULL)
            {
                *in_redirect = '\0';
                in_redirect++;

                while (*in_redirect == ' ')
                    in_redirect++;

                input_file = strtok(in_redirect, " ");
            }

            if (out_redirect != NULL)
            {
                *out_redirect = '\0';
                out_redirect++;

                while (*out_redirect == ' ')
                    out_redirect++;

                output_file = strtok(out_redirect, " ");
            }

            if (i > 0)
            {
                dup2(pipes[i - 1][0], STDIN_FILENO);
            }

            if (i < command_count - 1)
            {
                dup2(pipes[i][1], STDOUT_FILENO);
            }

            if (input_file != NULL)
            {
                int fd = open(input_file, O_RDONLY);

                if (fd == -1)
                {
                    perror("open input");
                    exit(1);
                }

                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            if (output_file != NULL)
            {
                int fd = open(output_file,
                              O_WRONLY | O_CREAT | O_TRUNC,
                              0644);

                if (fd == -1)
                {
                    perror("open output");
                    exit(1);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            for (int j = 0; j < command_count - 1; j++)
            {
                close(pipes[j][0]);
                close(pipes[j][1]);
            }

            while (*commands[i] == ' ')
                commands[i]++;

            char *args[MAX_ARGS];

            parse_args(commands[i], args);

            execvp(args[0], args);

            perror("execvp");
            exit(1);
        }
    }

    for (int i = 0; i < command_count - 1; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }

    for (int i = 0; i < command_count; i++)
    {
        waitpid(pids[i], NULL, 0);
    }
}

int main()
{
    char input[MAX_INPUT];

    printf("===== Skill 15 =====\n");
    printf("Enter command: ");

    fgets(input, sizeof(input), stdin);
    input[strcspn(input, "\n")] = '\0';

    execute_pipeline(input);

    return 0;
}
