#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <ctype.h>

#define MAX_LINE 1024
#define MAX_ARGS 100  
 
int main() {
    char line[MAX_LINE];
    while (1) { 
        printf("myshell> ");
        fflush(stdout);      
        if (!fgets(line, sizeof(line), stdin))
            break;   
 
        line[strcspn(line, "\n")] = 0;

        if (strlen(line) == 0)
            continue;

        // ============================================
        char *args[MAX_ARGS];
        char *input_file = NULL;
        char *output_file = NULL;
        int append = 0;
        int argc = 0;

        char *ptr = line;

        // Parse input manually 
        while (*ptr) {

            // Skip spaces
            while (isspace(*ptr))
                ptr++;

            if (*ptr == '\0')
                break;

            // Handle quoted strings
            if (*ptr == '"') {
                ptr++;
                args[argc++] = ptr;

                while (*ptr && *ptr != '"')
                    ptr++;

                if (*ptr == '"') {
                    *ptr = '\0';
                    ptr++;
                }
            }

            // Handle input redirection
            else if (*ptr == '<') {
                ptr++;
                while (isspace(*ptr)) ptr++;
                input_file = ptr;

                while (*ptr && !isspace(*ptr))
                    ptr++;

                if (*ptr) {
                    *ptr = '\0';
                    ptr++;
                }
            }

            // Handle output redirection 
            else if (*ptr == '>') {
                ptr++;

                if (*ptr == '>') {
                    append = 1;
                    ptr++;
                } else {
                    append = 0;
                }

                while (isspace(*ptr)) ptr++;
                output_file = ptr;

                while (*ptr && !isspace(*ptr))
                    ptr++;

                if (*ptr) {
                    *ptr = '\0';
                    ptr++;
                }
            }

            // Handle normal arguments
            else {
                args[argc++] = ptr;

                while (*ptr && !isspace(*ptr))
                    ptr++;

                if (*ptr) {
                    *ptr = '\0';
                    ptr++;
                }
            }
        }

        args[argc] = NULL;  

        if (argc == 0)
            continue;

        // Built in command: exit
        if (strcmp(args[0], "exit") == 0) {
            break;
        }

        // Built in command: cd
        if (strcmp(args[0], "cd") == 0) {

            if (args[1] == NULL) {
                chdir(getenv("HOME"));
            } else {
                if (chdir(args[1]) != 0)
                    perror("cd error");
            }

            continue;
        }

        // Create child process
        pid_t pid = fork();

        if (pid < 0) {
            perror("fork failed");
            continue;
        }

        if (pid == 0) {

            // Input redirection
            if (input_file != NULL) {
                int fd = open(input_file, O_RDONLY);
                if (fd < 0) {
                    perror("input file error");
                    exit(1);
                }
                dup2(fd, STDIN_FILENO);
                close(fd);
            }

            // Output redirection
            if (output_file != NULL) {
                int fd;

                if (append)
                    fd = open(output_file, O_WRONLY | O_CREAT | O_APPEND, 0644);
                else
                    fd = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);

                if (fd < 0) {
                    perror("output file error");
                    exit(1);
                }

                dup2(fd, STDOUT_FILENO);
                close(fd);
            }

            // Execute command
            execvp(args[0], args);

            // If exec fails
            perror("command not found");
            exit(1);
        }
        else {
            // Parent waits for child
            waitpid(pid, NULL, 0);
        }
    }
    
    printf("\nGoodbye!\n");
    return 0;
}
