#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_SIZE 100

void memory_test()
{
    char *buffer = malloc(MAX_SIZE);

    if (buffer == NULL)
    {
        perror("malloc");
        return;
    }

    strcpy(buffer, "OSSP Skill 22 Memory Test");

    printf("Allocated memory: %s\n", buffer);

    free(buffer);
    buffer = NULL;

    printf("Memory released successfully\n");
}

void input_test()
{
    char input[MAX_SIZE];

    printf("Enter a test command: ");
    fgets(input, sizeof(input), stdin);

    input[strcspn(input, "\n")] = '\0';

    if (strlen(input) == 0)
    {
        printf("Test failed: Empty input\n");
        return;
    }

    printf("Test passed: %s\n", input);
}

int main()
{
    printf("===== Skill 22 =====\n\n");

    printf("Memory Test\n");
    memory_test();

    printf("\nInput Test\n");
    input_test();

    printf("\nAll tests completed\n");

    return 0;
}
