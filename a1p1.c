#include <stdio.h>

int main(){
	
    int matrix[100][1000];

    // Read and store matrix from input file
    for (int row = 0; row < 100; row++) {
        for (int col = 0; col < 1000; col++) {
            // Ensures valid input along the way
            if (scanf("%d", &matrix[row][col]) != 1) {
                fprintf(stderr, "Error: bad input at coordinate [%d,%d]\n", row, col);
                return 1;
            }
        }
    }
    // test
    printf("In 'test1' there should be a 1 at coordinate 36, 309. Value read at row 36 col 309: %d\n", matrix[36][309]);

}