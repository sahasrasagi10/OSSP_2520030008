#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void execute_pwd()
{
    char cwd[1024];

    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        printf("Current directory: %s\n", cwd);
    }
    else
    {
        perror("pwd");
    }
}

void execute_export(char *argument)
{
    char *equal;

    if (argument == NULL)
    {
        printf("export: missing argument\n");
        return;
    }

    // Find '='
    equal = strchr(argument, '=');

    if (equal == NULL)
    {
        printf("export: invalid syntax\n");
        return;
    }

    // Separate variable name and value
    *equal = '\0';

    char *name = argument;
    char *value = equal + 1;

    // Check variable name
    if (strlen(name) == 0)
    {
        printf("export: invalid variable name\n");
        return;
    }

    // Set environment variable
    if (setenv(name, value, 1) == -1)
    {
        perror("export");
        return;
    }

    printf("Exported: %s=%s\n", name, value);
}

void execute_exit()
{
    printf("Exiting shell...\n");

    // Cleanup can be performed here
    // before terminating the program.

    exit(0);
}

int main()
{
    char command[100];
    char argument[200];

    printf("===== Skill 10: Built-in Commands =====\n");

    while (1)
    {
        printf("\nEnter command (pwd / export / exit): ");
        scanf("%s", command);

        if (strcmp(command, "pwd") == 0)
        {
            execute_pwd();
        }

        else if (strcmp(command, "export") == 0)
        {
            scanf("%s", argument);
            execute_export(argument);

            // Demonstrate that variable was stored
            char *value = getenv(argument);

            if (value != NULL)
            {
                printf("Environment value: %s\n", value);
            }
        }

        else if (strcmp(command, "exit") == 0)
        {
            execute_exit();
        }

        else
        {
            printf("Unknown command: %s\n", command);
        }
    }

    return 0;
}
