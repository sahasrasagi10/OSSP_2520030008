#include <stdio.h>
#include <string.h>

int main() {
    char input[200];

    printf("Enter a command/string: ");
    fgets(input, sizeof(input), stdin);

    input[strcspn(input, "\n")] = '\0';

    printf("\nOriginal Input: %s\n", input);

    printf("\nSingle Quotes:\n");
    printf("Single quotes preserve literal content and ignore variable expansion.\n");

    printf("\nDouble Quotes:\n");
    printf("Double quotes preserve spaces and allow variable expansion.\n");

    printf("\nParsed Result: %s\n", input);

    return 0;
}
