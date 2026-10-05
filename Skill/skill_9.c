#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>

char previous_dir[1024] = "";

void change_directory(char *path)
{
    char current_dir[1024];

    // Save current directory
    if (getcwd(current_dir, sizeof(current_dir)) == NULL)
    {
        perror("getcwd");
        return;
    }

    // If no argument, go to HOME
    if (path == NULL)
    {
        path = getenv("HOME");
    }

    // cd -
    if (strcmp(path, "-") == 0)
    {
        if (strlen(previous_dir) == 0)
        {
            printf("cd: previous directory not set\n");
            return;
        }

        path = previous_dir;
    }

    // Change directory
    if (chdir(path) == -1)
    {
        perror("cd");
        return;
    }

    // Store old directory
    strcpy(previous_dir, current_dir);
}
