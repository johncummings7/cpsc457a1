#include <stdio.h>
#include <stdlib.h> // for exit calls
#include <sys/types.h> // for pid_t type
#include <sys/wait.h> // for wait calls
#include <unistd.h> // for fork()

int main(){
	
    int matrix[100][1000];
    pid_t row_pid[100]; // Array of pid's, size 100 for each child

    // Read and store matrix from input file
    for (int row = 0; row < 100; row++) {
        for (int col = 0; col < 1000; col++) {
            // Ensures valid input along the way
            if (scanf("%d", &matrix[row][col]) != 1) {
                fprintf(stderr, "Error: bad input at coordinate [%d,%d]\n", row, col);
                return 1;
            }

            // test
            if (matrix[row][col] == 1) {
                printf("Found 1 at row: %d col: %d\n", row, col);
            }
        }
    }

    fflush(NULL); // flushing io streams

    // Testing process creation: forking 100 children
    for (int row = 0; row < 100; row++){
        pid_t fr = fork();

        if (fr < 0) { // Error occurred 
            fprintf(stderr, "Fork FAILED at row %d\n", row);
            exit (-1);
        }
        if (fr == 0) { // Child process
            printf("Child %d (PID %d): started\n", row, getpid());
            exit(0);
        } else { // Parent process
            row_pid[row] = fr;
            printf("Parent: forked child %d with PID %d\n", row, fr);
        }
    }

    // Making sure processes exit
    int status;
    for (int i = 0; i < 100; i++) {
        pid_t finished_pid = wait(&status);
        printf("Parent: child with PID %d exited with status %d\n", finished_pid, WEXITSTATUS(status));
    }

     
    return 0;
}