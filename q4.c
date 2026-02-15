#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>

int frequency = 800;

void handle_sigint(int sig) {
    const char *msg = "\n[WARNING] Carrier Interrupt Signal Received\n";
    write(STDOUT_FILENO, msg, strlen(msg));
    frequency = 800;

    const char *prompt = "station-controller$ ";
    write(STDOUT_FILENO, prompt, strlen(prompt));
}

int main() {
    char *line = NULL;
    size_t len = 0;
    ssize_t read_len;

    struct sigaction sa;
    sa.sa_handler = handle_sigint;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;

    if (sigaction(SIGINT, &sa, NULL) == -1) {
        perror("sigaction");
        return 1;
    }

    while (1) {
        printf("station-controller$ ");
        fflush(stdout);

        errno = 0;
        read_len = getline(&line, &len, stdin);

        if (read_len == -1) {
            if (errno == EINTR) {
                clearerr(stdin);
                continue;
            }
            if (feof(stdin)) {
                printf("\n");
                break;
            }
            perror("getline");
            break;
        }

        if (read_len > 0 && line[read_len - 1] == '\n')
            line[read_len - 1] = '\0';

        char *args[64];
        int arg_count = 0;
        char *token = strtok(line, " ");
        while (token != NULL && arg_count < 63) {
            args[arg_count++] = token;
            token = strtok(NULL, " ");
        }
        args[arg_count] = NULL;

        if (arg_count == 0) continue;

        if (strcmp(args[0], "set_freq") == 0) {
            if (arg_count < 2) {
                printf("Error: missing frequency argument\n");
            } else {
                int new_freq = atoi(args[1]);
                if (new_freq <= 0)
                    printf("Error: Frequency must be a positive integer\n");
                else
                    frequency = new_freq;
            }
        } else if (strcmp(args[0], "get_freq") == 0) {
            printf("Current Frequency: %d MHz\n", frequency);
        } else if (strcmp(args[0], "quit") == 0) {
            break;
        } else if (strcmp(args[0], "top") == 0) {
            pid_t pid = fork();
            if (pid == 0) {
                char *cmd_args[] = {"top", NULL};
                execvp("top", cmd_args);
                perror("execvp");
                exit(1);
            } else if (pid > 0) {
                int status;
                while (waitpid(pid, &status, 0) == -1) {
                    if (errno != EINTR) {
                        perror("waitpid");
                        break;
                    }
                }
            } else {
                perror("fork");
            }
        } else if (strcmp(args[0], "ping") == 0) {
            if (arg_count < 2) {
                fprintf(stderr, "Error: missing address argument\n");
            } else {
                pid_t pid = fork();
                if (pid == 0) {
                    char *cmd_args[] = {"ping", "-c", "4", args[1], NULL};
                    execvp("ping", cmd_args);
                    perror("execvp");
                    exit(1);
                } else if (pid > 0) {
                    int status;
                    while (waitpid(pid, &status, 0) == -1) {
                        if (errno != EINTR) {
                            perror("waitpid");
                            break;
                        }
                    }
                } else {
                    perror("fork");
                }
            }
        } else {
            printf("Error: Command not found: %s\n", args[0]);
        }
    }

    free(line);
    return 0;
}
