#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>

#define MAX_JOBS 20
#define MAX_COMMAND 200

typedef struct
{
    int job_id;
    pid_t pid;
    char command[MAX_COMMAND];
    int running;
    int stopped;
} Job;

Job jobs[MAX_JOBS];
int job_count = 0;
int next_job_id = 1;

void update_jobs()
{
    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].running)
        {
            int status;

            pid_t result = waitpid(
                jobs[i].pid,
                &status,
                WNOHANG | WUNTRACED
            );

            if (result > 0)
            {
                if (WIFSTOPPED(status))
                {
                    jobs[i].stopped = 1;
                }
                else if (WIFEXITED(status) ||
                         WIFSIGNALED(status))
                {
                    jobs[i].running = 0;
                    jobs[i].stopped = 0;
                }
            }
        }
    }
}

void add_job(pid_t pid, char *command)
{
    if (job_count >= MAX_JOBS)
    {
        printf("Job table full\n");
        return;
    }

    jobs[job_count].job_id = next_job_id++;
    jobs[job_count].pid = pid;
    strcpy(jobs[job_count].command, command);
    jobs[job_count].running = 1;
    jobs[job_count].stopped = 0;

    printf("[%d] %d %s\n",
           jobs[job_count].job_id,
           pid,
           command);

    job_count++;
}

void list_jobs()
{
    update_jobs();

    printf("\nActive Jobs:\n");

    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].running)
        {
            if (jobs[i].stopped)
            {
                printf("[%d] Stopped  PID=%d  %s\n",
                       jobs[i].job_id,
                       jobs[i].pid,
                       jobs[i].command);
            }
            else
            {
                printf("[%d] Running  PID=%d  %s\n",
                       jobs[i].job_id,
                       jobs[i].pid,
                       jobs[i].command);
            }
        }
    }
}

void execute_background(char *command)
{
    pid_t pid = fork();

    if (pid < 0)
    {
        perror("fork");
        return;
    }

    if (pid == 0)
    {
        char *args[50];
        int count = 0;

        char temp[MAX_COMMAND];
        strcpy(temp, command);

        char *token = strtok(temp, " ");

        while (token != NULL && count < 49)
        {
            args[count++] = token;
            token = strtok(NULL, " ");
        }

        args[count] = NULL;

        execvp(args[0], args);

        perror("execvp");
        exit(1);
    }

    add_job(pid, command);
}

void bring_to_foreground(int job_id)
{
    update_jobs();

    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].job_id == job_id &&
            jobs[i].running)
        {
            printf("Bringing job [%d] to foreground\n",
                   jobs[i].job_id);

            if (jobs[i].stopped)
            {
                kill(jobs[i].pid, SIGCONT);
                jobs[i].stopped = 0;
            }

            waitpid(jobs[i].pid, NULL, 0);

            jobs[i].running = 0;

            printf("Job [%d] completed\n",
                   jobs[i].job_id);

            return;
        }
    }

    printf("Job [%d] not found\n", job_id);
}

int main()
{
    char input[MAX_COMMAND];

    printf("===== Skill 17 =====\n");

    while (1)
    {
        update_jobs();

        printf("\nmyshell> ");

        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "jobs") == 0)
        {
            list_jobs();
        }
        else if (strncmp(input, "fg ", 3) == 0)
        {
            int job_id = atoi(input + 3);
            bring_to_foreground(job_id);
        }
        else if (strcmp(input, "exit") == 0)
        {
            break;
        }
        else
        {
            int length = strlen(input);

            if (length > 0 &&
                input[length - 1] == '&')
            {
                input[length - 1] = '\0';

                while (strlen(input) > 0 &&
                       input[strlen(input) - 1] == ' ')
                {
                    input[strlen(input) - 1] = '\0';
                }

                execute_background(input);
            }
            else
            {
                printf("Use '&' for background jobs\n");
            }
        }
    }

    return 0;
}
