#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <time.h>
#include <sys/wait.h>

void print_factors(int num) {
    printf("Child: Factors of %d: ", num);
    for (int i = 1; i <= num; i++) {
        if (num % i == 0)
            printf("%d ", i);
    }
    printf("\n");
    fflush(stdout);
}

int main() {
    int n;
    printf("Enter n: ");
    if (scanf("%d", &n) != 1 || n <= 0) {
        printf("n must be greater than 0.\n");
        return -1;
    }

    int arr_size;
    printf("Enter number of elements in array: ");
    if (scanf("%d", &arr_size) != 1 || arr_size <= 0) {
        printf("Array size must be greater than 0.\n");
        return -1;
    }

    int arr[arr_size];
    printf("Enter %d integers: ", arr_size);
    for (int i = 0; i < arr_size; i++) {
        if (scanf("%d", &arr[i]) != 1) {
            printf("Invalid integer input.\n");
            return -1;
        }
    }

    int visited[arr_size];
    for (int i = 0; i < arr_size; i++) {
        visited[i] = 0;
    }

    int pipefd[2];
    if (pipe(pipefd) == -1) {
        perror("pipe");
        return -1;
    }

    pid_t pid = fork();
    if (pid == -1) {
        perror("fork");
        return -1;
    }

    if (pid == 0) {
        close(pipefd[1]);

        int value;
        while (read(pipefd[0], &value, sizeof(value)) > 0) {
            if (value == -1)
                break;

            print_factors(value);
            sleep(time(NULL) % n);
        }

        close(pipefd[0]);
        exit(0);
    } else {
        close(pipefd[0]);
        srand(time(NULL));

        int visited_count = 0;

        while (visited_count < arr_size) {
            int idx = rand() % arr_size;
            int value = arr[idx];

            if (!visited[idx]) {
                visited[idx] = 1;
                visited_count++;
            }

            printf("Parent: Selected %d\n", value);
            fflush(stdout);

            if (write(pipefd[1], &value, sizeof(value)) == -1) {
                perror("write");
                break;
            }

            sleep(value % n);
        }

        int terminate = -1;
        write(pipefd[1], &terminate, sizeof(terminate));

        close(pipefd[1]);
        wait(NULL);
    }

    return 0;
}
