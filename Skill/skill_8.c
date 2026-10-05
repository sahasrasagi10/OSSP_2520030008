#define _POSIX_C_SOURCE 200809L

#include <stdio.h>
#include <string.h>

#define MAX_LINE 1024
#define MAX_ARGS 64

/* The parsed command and its arguments form the execution structure. */
typedef struct {
    size_t argc;
    char *argv[MAX_ARGS + 1];
} CommandNode;

typedef enum {
    PARSE_EMPTY,
    PARSE_OK,
    PARSE_ERROR
} ParseStatus;

static int is_unsupported_operator(char ch) {
    return ch == '|' || ch == '&' || ch == ';' ||
           ch == '<' || ch == '>';
}

static ParseStatus parse_command(
    char *line,
    CommandNode *command,
    char *error,
    size_t error_size
) {
    command->argc = 0;
    error[0] = '\0';

    char *saveptr = NULL;
    char *token = strtok_r(line, " \t\r\n", &saveptr);

    while (token != NULL) {
        for (const char *p = token; *p != '\0'; ++p) {
            if (is_unsupported_operator(*p)) {
                snprintf(error, error_size,
                         "operator '%c' is not supported yet", *p);
                return PARSE_ERROR;
            }
        }

        if (command->argc >= MAX_ARGS) {
            snprintf(error, error_size, "too many command arguments");
            return PARSE_ERROR;
        }

        command->argv[command->argc++] = token;
        token = strtok_r(NULL, " \t\r\n", &saveptr);
    }

    if (command->argc == 0) {
        return PARSE_EMPTY;
    }

    command->argv[command->argc] = NULL;
    return PARSE_OK;
}

static void print_parse_tree(const CommandNode *command) {
    puts("\nParse tree:");
    puts("COMMAND");
    printf("  PROGRAM: %s\n", command->argv[0]);

    for (size_t i = 1; i < command->argc; ++i) {
        printf("  ARGUMENT[%zu]: %s\n", i, command->argv[i]);
    }

    puts("\nExecution structure:");
    for (size_t i = 0; i < command->argc; ++i) {
        printf("  argv[%zu] = \"%s\"\n", i, command->argv[i]);
    }
    printf("  argv[%zu] = NULL\n", command->argc);
}

int main(void) {
    char input[MAX_LINE];

    printf("myshell> ");
    fflush(stdout);

    if (fgets(input, sizeof(input), stdin) == NULL) {
        return 0;
    }

    if (strchr(input, '\n') == NULL && !feof(stdin)) {
        int ch;
        while ((ch = getchar()) != '\n' && ch != EOF) {
            /* Discard the rest of the overlong input line. */
        }
        fprintf(stderr, "Syntax error: input line is too long\n");
        return 1;
    }

    CommandNode command;
    char error[128];

    ParseStatus result =
        parse_command(input, &command, error, sizeof(error));

    if (result == PARSE_EMPTY) {
        puts("Empty command: nothing to parse.");
        return 0;
    }

    if (result == PARSE_ERROR) {
        fprintf(stderr, "Syntax error: %s\n", error);
        return 1;
    }

    print_parse_tree(&command);
    return 0;
}
