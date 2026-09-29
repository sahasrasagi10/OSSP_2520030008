#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <time.h>
#include <limits.h>
#include <errno.h>

void showHelp() {
    printf("\n");
    printf("============================================================\n");
    printf("                    SHELLFORGE COMMANDS\n");
    printf("============================================================\n");
    printf("  1. ls              List files and directories\n");
    printf("  2. pwd             Show current directory\n");
    printf("  3. mkdir <name>    Create a new directory\n");
    printf("  4. cd <name>       Change directory\n");
    printf("  5. date            Show current date and time\n");
    printf("  6. cal             Show calendar\n");
    printf("  7. touch <file>    Create an empty file\n");
    printf("  8. rm <file>       Delete a file\n");
    printf("  9. clear           Clear terminal\n");
    printf(" 10. exit            Exit ShellForge\n");
    printf("============================================================\n");
}

int main() {

    char command[200];
    char hostname[100];
    char currentPath[PATH_MAX];

    char username[] = "2520030008_Sahasra";

    if (gethostname(hostname, sizeof(hostname)) != 0) {
        strcpy(hostname, "localhost");
    }

    printf("\n");
    printf("============================================================\n");
    printf("                    WELCOME TO SHELLFORGE\n");
    printf("============================================================\n");
    printf("             A Simple Linux Command Shell\n");
    printf("============================================================\n");
    printf("Type 'help' to see available commands.\n");
    printf("============================================================\n");

    while (1) {

        if (getcwd(currentPath, sizeof(currentPath)) == NULL) {
            strcpy(currentPath, "?");
        }

        printf("\n");
        printf("\033[1;36m%s@%s\033[0m:\033[1;32m%s\033[0m$ ",
               username,
               hostname,
               currentPath);

        if (fgets(command, sizeof(command), stdin) == NULL) {
            break;
        }

        command[strcspn(command, "\n")] = '\0';

        if (strcmp(command, "ls") == 0) {

            DIR *dir;
            struct dirent *entry;

            dir = opendir(".");

            if (dir == NULL) {
                printf("Unable to open directory.\n");
                continue;
            }

            printf("\n");
            printf("Contents of current directory:\n");
            printf("------------------------------------------------------------\n");

            while ((entry = readdir(dir)) != NULL) {

                if (entry->d_name[0] != '.') {
                    printf("%-25s", entry->d_name);
                }
            }

            printf("\n");
            closedir(dir);
        }

        else if (strcmp(command, "pwd") == 0) {

            if (getcwd(currentPath, sizeof(currentPath)) != NULL) {
                printf("\nCurrent Directory:\n%s\n", currentPath);
            }
            else {
                printf("Unable to get current directory.\n");
            }
        }

        else if (strncmp(command, "mkdir ", 6) == 0) {

            char *dirname = command + 6;

            if (strlen(dirname) == 0) {
                printf("mkdir: missing directory name\n");
            }

            else if (mkdir(dirname, 0777) == 0) {
                printf("Directory '%s' created successfully.\n", dirname);
            }

            else {
                if (errno == EEXIST) {
                    printf("mkdir: '%s' already exists.\n", dirname);
                }
                else {
                    printf("mkdir: unable to create directory '%s'.\n",
                           dirname);
                }
            }
        }

        else if (strncmp(command, "cd ", 3) == 0) {

            char *dirname = command + 3;

            if (strlen(dirname) == 0) {
                printf("cd: missing directory name\n");
            }

            else if (chdir(dirname) == 0) {

                if (getcwd(currentPath, sizeof(currentPath)) != NULL) {
                    printf("Changed directory to:\n%s\n", currentPath);
                }
            }

            else {
                printf("cd: unable to access '%s'\n", dirname);
            }
        }

        else if (strcmp(command, "date") == 0) {

            time_t currentTime;

            time(&currentTime);

            printf("\nCurrent Date and Time:\n");
            printf("%s", ctime(&currentTime));
        }

        else if (strcmp(command, "cal") == 0) {

            printf("\n");
            system("cal");
        }

        else if (strncmp(command, "touch ", 6) == 0) {

            char *filename = command + 6;

            if (strlen(filename) == 0) {
                printf("touch: missing file name\n");
            }

            else {

                FILE *file = fopen(filename, "a");

                if (file == NULL) {
                    printf("touch: unable to create '%s'\n", filename);
                }

                else {
                    fclose(file);
                    printf("File '%s' created successfully.\n", filename);
                }
            }
        }

        else if (strncmp(command, "rm ", 3) == 0) {

            char *filename = command + 3;

            if (strlen(filename) == 0) {
                printf("rm: missing file name\n");
            }

            else if (remove(filename) == 0) {
                printf("File '%s' deleted successfully.\n", filename);
            }

            else {
                printf("rm: unable to delete '%s'\n", filename);
            }
        }

        else if (strcmp(command, "clear") == 0) {

            system("clear");
        }

        else if (strcmp(command, "help") == 0) {

            showHelp();
        }

        else if (strcmp(command, "exit") == 0) {

            printf("\n============================================================\n");
            printf("              Exiting ShellForge...\n");
            printf("              Thank you for using ShellForge!\n");
            printf("============================================================\n");

            break;
        }

        else if (strlen(command) == 0) {

            continue;
        }

        else {

            printf("\nShellForge: command not found: %s\n", command);
            printf("Type 'help' to see available commands.\n");
        }
    }

    return 0;
}
