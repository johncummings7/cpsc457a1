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
            printf("Child %d (PID %d): Searching row %d\n", row, getpid(), row);

            // Child starts scanning row for a 1, updates treasure_found flag if found
            int treasure_found = 0;
            for (int col = 0; col < 1000; col++) {
                if (matrix[row][col] == 1) {
                    treasure_found = 1;
                    break; // No need to keep scanning when treasure is found, se we exit loop
                }
            }

            exit(treasure_found); // 1 if found, 0 otherwise
        } else { // Parent process
            // Puts every childs PID in an array for later search
            row_pid[row] = fr;
        }
    }

    // Making sure processes exit (waiting)
    // Variables used to assign where the treasure will have been found. 
    // Both at -1 because we don't yet know where the treasure was found.
    int found_row = -1, found_col = -1; 
    pid_t found_pid = -1; // Used to assign the PID that found the treasure

    int status;
    for (int i = 0; i < 100; i++) {
        pid_t finished_pid = wait(&status); // Gets PID of child that just exited, status should now know how the child process ended
        // Error handling
        if (finished_pid == -1){
            perror("Wait");
            exit(1);
        }

        int exit_status = WEXITSTATUS(status); // Get childs exit code (0 or 1)

        /*
        Finding the row corelating to the PID that just exited.
        The parent proccess stores each childs PID (value in fr) into row_pid[].
        We iterate through row_pid[] until we find the PID that matches the PID that just exited.
        The index of the matching PID in row_pid[] cooresponds to the row that the child was searching.
        */ 
        int row = -1; 
        for (int r = 0; r < 100; r++) {
            if (row_pid[r] == finished_pid) {
                // We now know the row that child with PID 'finished_pid' searched, so we can exit the loop
                row = r;
                break;
            }
        }

        /*
        If the child that just exited reports a success (exit_status is 1), iterate through each column
        along the row it was searching until the treasure is found, then mark its location and update
        found_pid. Should only be entering this block once (good) as found_row is changed within it.
        */ 
        if (exit_status == 1 && found_row == -1 && row != -1) {
            for (int c = 0; c < 1000; c++) {
                if (matrix[row][c] == 1) {
                    found_col = c;
                    break;
                }
            }
            found_row = row;
            found_pid = finished_pid;
        }
    }

    if (found_row != -1) {
        printf("Parent: The treasure was found by child with PID %d at row %d and column %d\n", found_pid, found_row, found_col);
    } else {
        printf("No treasure was found in the matrix\n");
    }

    return 0;
}