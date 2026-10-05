#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>

int main() {
    char *path = getenv("PATH");

    if (path == NULL) {
        printf("PATH variable not found.\n");
        return 1;
    }

    printf("PATH Variable:\n%s\n\n", path);

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork failed");
        return 1;
    }

    if (pid == 0) {
        // Child process
        printf("Child Process Started\n");
        printf("Child PID: %d\n", getpid());

        // Execute ls
        execlp("ls", "ls", NULL);

        // Runs only if exec fails
        perror("exec failed");
        exit(1);
    } else {
        // Parent process
        int status;

        printf("Parent waiting for child...\n");
        waitpid(pid, &status, 0);

        if (WIFEXITED(status)) {
            printf("Child exited with status: %d\n", WEXITSTATUS(status));
        }
    }

    return 0;
}
