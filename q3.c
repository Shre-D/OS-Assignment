#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Usage: %s <pattern> <file1> [file2 ...]\n", argv[0]);
        return 1;
    }


    int pipe1[2]; 
    int pipe2[2]; 

    if (pipe(pipe1) == -1) {
        perror("pipe1");
        return 1;
    }

    if (fork() == 0) {
        dup2(pipe1[1], STDOUT_FILENO);
        close(pipe1[0]);
        close(pipe1[1]);

        char *grep_args[64];
        grep_args[0] = "grep";
        grep_args[1] = "-n";
        grep_args[2] = "-h";
        
        for (int i = 1; i < argc; i++) {
            grep_args[i + 2] = argv[i];
        }
        grep_args[argc + 2] = NULL;

        execvp("grep", grep_args);
        perror("execvp");
        exit(1);
    }

    if (pipe(pipe2) == -1) {
        perror("pipe2");
        return 1;
    }

    if (fork() == 0) {
        dup2(pipe1[0], STDIN_FILENO);
        dup2(pipe2[1], STDOUT_FILENO);
        
        close(pipe1[0]);
        close(pipe1[1]);
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("cut", "cut", "-d:", "-f1", NULL);
        perror("execlp cut");
        exit(1);
    }

    close(pipe1[0]);
    close(pipe1[1]);

    if (fork() == 0) {
        dup2(pipe2[0], STDIN_FILENO);
        
        close(pipe2[0]);
        close(pipe2[1]);

        execlp("sort", "sort", "-u", "-n", NULL);
        perror("execlp sort");
        exit(1);
    }

    close(pipe2[0]);
    close(pipe2[1]);

    wait(NULL);
    wait(NULL);
    wait(NULL);

    return 0;
}
