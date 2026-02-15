#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <string.h>


void print_memory_usage(int k) {
    char cmd[256];
    snprintf(cmd, sizeof(cmd),
             "ps axo user,pid,pmem,time --sort=-pmem | head -n %d",
             k + 1);
    system(cmd);
}


int main() {
    int n, k, r;
    printf("Enter n (seconds), k (processes), r (iterations): ");
    if (scanf("%d %d %d", &n, &k, &r) != 3 || n <= 0 || k <= 0 || r <= 0) {
        printf("Invalid input.\n");
        return 1;
    }

    int pipe1[2]; 
    int pipe2[2]; 

    if (pipe(pipe1) == -1 || pipe(pipe2) == -1) {
        perror("pipe");
        return 1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return 1;
    }

    if (pid == 0) {
        close(pipe1[1]); 
        close(pipe2[0]); 

        int iteration = 0;
        while (1) {
            print_memory_usage(k);
            iteration++;

            if (iteration % r == 0) {
                char dummy = 'R';
                write(pipe2[1], &dummy, 1);

                int target_pid;
                if (read(pipe1[0], &target_pid, sizeof(target_pid)) > 0) {
                    if (target_pid == -2) {
                        break;
                    } else if (target_pid == -1) {
                    } else {
                        if (kill(target_pid, SIGTERM) == -1) {
                            perror("kill");
                        }
                    }
                } else {
                    break; 
                }
            }

            sleep(n);
        }

        close(pipe1[0]);
        close(pipe2[1]);
        exit(0);

    } else {
        close(pipe1[0]); 
        close(pipe2[1]); 

        char signal_byte;
        while (read(pipe2[0], &signal_byte, 1) > 0) {
            int target_pid;
            printf("Parent: Enter PID to kill, -1 to skip, -2 to exit: ");
            scanf("%d", &target_pid);

            write(pipe1[1], &target_pid, sizeof(target_pid));

            if (target_pid == -2) {
                break;
            }
        }

        close(pipe1[1]);
        close(pipe2[0]);
        wait(NULL); 
    }

    return 0;
}
