#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/wait.h>


/*
Side note: I am using longs for parsing and arithmetic at the beginning so can utilize the strtol() function
and avoid any undefined behaviour, and save headache with bound checking and parsing.
https://stackoverflow.com/questions/7021725/how-to-convert-a-string-to-integer-in-c
*/

/*
Helper to parse inputs to longs.
Takes the string to parse, s, and the label for what that string is meant to be (UPPER, LOWER, or N).
*/
static long string_to_long(const char *s, const char *label) {
    char *end = NULL; // Pointer used to find endpoint of the string (\0)
    long num = strtol(s, &end, 10); // Converting string to base 10 long

    // Checking that digits were parsed and that the string ends with \0 identifier
    if (end == s || *end != '\0') {
        fprintf(stderr, "Invalid %s: %s\n", label, s);
        exit(1);
    }
    return num;
}

int main(int argc, char *argv[]) {
    if (argc != 4) { // Error handling: looking for 3 command line arguments
        fprintf(stderr, "Bad input, usage is: %s LOWER_BOUND UPPER_BOUND N\n", argv[0]);
        return 1;
    }

    // Initializing bounds from command line using helper
    long LOWER = string_to_long(argv[1], "LOWER_BOUND");
    long UPPER = string_to_long(argv[2], "UPPER_BOUND");
    long N = string_to_long(argv[3], "N");

    // More error handing
    if (N < 1) {
        fprintf(stderr, "N must be greater than 0.\n");
        return 1;
    }
    if (LOWER > UPPER) {
        fprintf(stderr, "LOWER bound must be <= UPPER bound.\n");
        return 1;
    }
    if (LOWER < 0 || UPPER < 0) {
        fprintf(stderr, "Bounds must be positive.\n");
        return 1;
    }

    long numbers_in_range = UPPER - LOWER + 1; // How many numbers to check
    
    if (N > numbers_in_range) { // Making sure N is not greater than the range
        printf("Setting N to %ld so each child process has at least one value to check.\n", numbers_in_range);
        N = numbers_in_range;
    }

    // Calculates amt of numbers each child will process, rounded up, using ceiling division
    // From https://stackoverflow.com/questions/2745074/fast-ceiling-of-an-integer-division-in-c-c
    long numbers_per_child = (numbers_in_range + (N-1)) / N;

    // Using suggested memory layout from assignment
    size_t block_size  = (size_t)numbers_per_child;
    size_t total = (size_t)N * block_size;
    const size_t SIZE = total * sizeof(int);

    // Creating shared memory
    int shmid = shmget(IPC_PRIVATE, SIZE, IPC_CREAT | 0666);
    if (shmid < 0) { // Terminate if something goes wrong with shmget
        perror("shmget");
        return 1;
    }

    // Attaching shared memory
    int *shm_ptr = (int *) shmat(shmid, NULL, 0);
    // Terminate if something goes wrong with shmat and use shmctl to stop memory leak
    if (shm_ptr == (void *)-1) { 
        perror("shmat");
        shmctl(shmid, IPC_RMID, NULL);
        return 1;
    }

    // Initializing shared memory with sentinels so parent knows where to stop reading in each segment
    for (size_t s = 0; s < total; s++) {
        shm_ptr[s] = -1;
    }

    // Forking N children
    for (long i = 0; i < N; i++) {
        pid_t fr = fork();

        if (fr < 0) {
            fprintf(stderr, "Fork FAILED\n");
            shmdt(shm_ptr);
            shmctl(shmid, IPC_RMID, NULL);
            return 1;
        }
        if (fr == 0) { // Child process
            long start = LOWER + i * numbers_per_child;
            long end = start + numbers_per_child - 1;
            if (end > UPPER) {
                end = UPPER;
            }

            printf("Child PID %d checking range [%ld, %ld]\n", getpid(), start, end);
            _exit(0);
        }
    }

    for (long i = 0; i < N; i++) {
        if (wait(NULL) < 0) {
            perror("wait");
        }
    }

    printf("Parent: All children finished. Primes found: (none yet because we arent looking)\n");

    if (shmdt(shm_ptr) == -1) {
        perror("shmdt");
    }

    if (shmctl(shmid, IPC_RMID, NULL) == -1) {
        perror("shmctl IPC_RMID");
    }

    return 0;
}