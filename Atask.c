#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>

int main() {

    // Total number of child processes
    const int numberOfChildren = 15;

    // Store child PIDs in the order they are created
    pid_t childPids[numberOfChildren];

    // Used to check how each child finishes
    int status;

    // Commands each child will run
    
    char *args[][3] = {
        {"ls", "-l", NULL},
        {"date", NULL, NULL},
        {"whoami", NULL, NULL},
        {"pwd", NULL, NULL},
        {"echo", "Hello Cindy", NULL},
        {"uname", "-a", NULL},
        {"id", NULL, NULL},
        {"uptime", NULL, NULL},
        {"ps", NULL, NULL},
        {"df", "-h", NULL},
        {"invalidcmd1", NULL, NULL},   //  invalid
        {"invalidcmd2", NULL, NULL},   //  invalid
        {"ls", NULL, NULL},
        {"date", NULL, NULL},
        {"echo", "CS470", NULL}
    };

    // Print parent process ID
    printf("Parent PID: %d\n", getpid());

    // Create child processes
    for (int i = 0; i < numberOfChildren; i++) {
        pid_t pid = fork();

        // Check if fork failed
        if (pid < 0) {
            perror("Fork failed");
            return 1;
        }

        // Child process
        if (pid == 0) {
            printf("Child #%d PID = %d executing: %s\n",
                   i, getpid(), args[i][0]);

            // These children intentionally terminate using a signal
            if (i == 13 || i == 14) {
                abort();
            }

            // Replace the child process with the command
            execvp(args[i][0], args[i]);

            // Only runs if execvp fails
            perror("execvp failed");
            exit(127);
        }

        // Parent saves the child's PID
        childPids[i] = pid;
    }

    // Counters for final summary
    int normalExitZero = 0;
    int normalExitNonZero = 0;
    int signalExit = 0;

    // Wait for children in the order they were created
    for (int i = 0; i < numberOfChildren; i++) {
        pid_t finishedPid = waitpid(childPids[i], &status, 0);

        // Child exited normally
        if (WIFEXITED(status)) {
            int code = WEXITSTATUS(status);
            printf("Child PID %d exited normally with code %d\n",
                   finishedPid, code);

            if (code == 0)
                normalExitZero++;
            else
                normalExitNonZero++;

        }
        // Child terminated by a signal
        else if (WIFSIGNALED(status)) {
            printf("Child PID %d terminated by signal %d\n",
                   finishedPid, WTERMSIG(status));
            signalExit++;
        }
    }

    // Print summary of how children finished
    printf("\nSummary:\n");
    printf("Normal exit (code 0): %d\n", normalExitZero);
    printf("Normal exit (non-zero code): %d\n", normalExitNonZero);
    printf("Terminated by signal: %d\n", signalExit);

    return 0;
}
