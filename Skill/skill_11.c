#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_HISTORY 10
#define MAX_COMMAND 100
#define MAX_PIPELINE 10

char history[MAX_HISTORY][MAX_COMMAND];
int history_count = 0;

char pipeline[MAX_PIPELINE][MAX_COMMAND];
int pipeline_count = 0;

void add_history(char *command)
{
    if (history_count < MAX_HISTORY)
    {
        strcpy(history[history_count], command);
        history_count++;
    }
    else
    {
        for (int i = 0; i < MAX_HISTORY - 1; i++)
        {
            strcpy(history[i], history[i + 1]);
        }

        strcpy(history[MAX_HISTORY - 1], command);
    }
}

void display_history()
{
    printf("\nCommand History:\n");

    for (int i = 0; i < history_count; i++)
    {
        printf("%d  %s\n", i + 1, history[i]);
    }
}

void create_pipeline(char *input)
{
    char temp[MAX_COMMAND];
    strcpy(temp, input);

    char *token = strtok(temp, "|");

    pipeline_count = 0;

    while (token != NULL && pipeline_count < MAX_PIPELINE)
    {
        while (*token == ' ')
            token++;

        strcpy(pipeline[pipeline_count], token);
        pipeline_count++;

        token = strtok(NULL, "|");
    }
}

void display_pipeline()
{
    printf("\nPipeline Structure:\n");

    for (int i = 0; i < pipeline_count; i++)
    {
        printf("Process %d: %s\n", i + 1, pipeline[i]);

        if (i < pipeline_count - 1)
        {
            printf("          |\n");
            printf("          v\n");
        }
    }
}

int main()
{
    char command[MAX_COMMAND];
    char pipeline_input[MAX_COMMAND];

    printf("===== Skill 11 =====\n");

    printf("\nEnter 3 commands:\n");

    for (int i = 0; i < 3; i++)
    {
        printf("Command %d: ", i + 1);

        fgets(command, sizeof(command), stdin);
        command[strcspn(command, "\n")] = '\0';

        add_history(command);
    }

    display_history();

    printf("\nEnter a pipeline command:\n");
    printf("Pipeline: ");

    fgets(pipeline_input, sizeof(pipeline_input), stdin);
    pipeline_input[strcspn(pipeline_input, "\n")] = '\0';

    create_pipeline(pipeline_input);
    display_pipeline();

    return 0;
}
