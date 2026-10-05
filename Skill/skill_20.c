#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>

#define MAX_COMMAND 200

pid_t shell_pgid;
pid_t foreground_pgid = -1;

void handle_sigtstp(int sig)
{
    if (foreground_pgid > 0)
        kill(-foreground_pgid, SIGTSTP);
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
        setpgid(0, 0);

        signal(SIGTSTP, SIG_DFL);
        signal(SIGINT, SIG_DFL);

        execvp(args[0], args);

        perror("execvp");
        exit(1);
    }

    setpgid(pid, pid);

    foreground_pgid = pid;

    tcsetpgrp(STDIN_FILENO, foreground_pgid);

    int status;

    waitpid(pid, &status, WUNTRACED);

    if (WIFSTOPPED(status))
    {
        printf("\nProcess stopped. PID: %d\n", pid);
    }

    tcsetpgrp(STDIN_FILENO, shell_pgid);

    foreground_pgid = -1;
}

int main()
{
    char input[MAX_COMMAND];

    shell_pgid = getpid();

    setpgid(shell_pgid, shell_pgid);

    tcsetpgrp(STDIN_FILENO, shell_pgid);

    signal(SIGTSTP, handle_sigtstp);
    signal(SIGINT, SIG_IGN);

    printf("===== Skill 20 =====\n");
    printf("PID: %d\n", shell_pgid);

    while (1)
    {
        printf("\nmyshell> ");
        fflush(stdout);

        if (fgets(input, sizeof(input), stdin) == NULL)
            break;

        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "exit") == 0)
            break;

        if (strlen(input) == 0)
            continue;

        execute_command(input);
    }

    return 0;
}
