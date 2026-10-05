#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

#define MAX_JOBS 20
#define MAX_COMMAND 200

typedef struct
{
    int job_id;
    pid_t pid;
    char command[MAX_COMMAND];
    int running;
} Job;

Job jobs[MAX_JOBS];
int job_count = 0;
int next_job_id = 1;

void add_job(pid_t pid, char *command)
{
    if (job_count >= MAX_JOBS)
    {
        printf("Job table is full\n");
        return;
    }

    jobs[job_count].job_id = next_job_id++;
    jobs[job_count].pid = pid;
    strcpy(jobs[job_count].command, command);
    jobs[job_count].running = 1;

    printf("[%d] %d running  %s\n",
           jobs[job_count].job_id,
           pid,
           command);

    job_count++;
}

void update_jobs()
{
    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].running)
        {
            int status;
            pid_t result = waitpid(jobs[i].pid, &status, WNOHANG);

            if (result > 0)
            {
                jobs[i].running = 0;
            }
        }
    }
}

void display_jobs()
{
    update_jobs();

    printf("\nActive Jobs:\n");

    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].running)
        {
            printf("[%d] Running    PID=%d    %s\n",
                   jobs[i].job_id,
                   jobs[i].pid,
                   jobs[i].command);
        }
    }
}

void remove_completed_jobs()
{
    int j = 0;

    for (int i = 0; i < job_count; i++)
    {
        if (jobs[i].running)
        {
            jobs[j++] = jobs[i];
        }
    }

    job_count = j;
}

void start_background_job(char *command)
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

int main()
{
    char input[MAX_COMMAND];

    printf("===== Skill 16 =====\n");

    while (1)
    {
        update_jobs();

        printf("\nmyshell> ");

        fgets(input, sizeof(input), stdin);
        input[strcspn(input, "\n")] = '\0';

        if (strcmp(input, "jobs") == 0)
        {
            display_jobs();
        }
        else if (strcmp(input, "clearjobs") == 0)
        {
            remove_completed_jobs();
            printf("Completed jobs removed\n");
        }
        else if (strcmp(input, "exit") == 0)
        {
            break;
        }
        else
        {
            int length = strlen(input);

            if (length > 0 && input[length - 1] == '&')
            {
                input[length - 1] = '\0';

                while (length > 1 && input[length - 2] == ' ')
                {
                    input[length - 2] = '\0';
                    length--;
                }

                start_background_job(input);
            }
            else
            {
                printf("Use '&' to run a background job\n");
            }
        }
    }

    return 0;
}
