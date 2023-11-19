#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <time.h>

#define N 9 // Size of the Sudoku (9x9)
#define SRN 3 // Square root of N, size of the small box (3x3 in standard Sudoku)
#define K 20 // Number of cells to be removed

// Function Declarations for SudokuSolver
int readAndValidateRow(int row);
void readSudoku();
int is_safe(int x, int y);
int solve_sudoku();
int validateSudoku();
void printMatrix();

// Function Declarations for SudokuGenerator
void fillDiagonal();
void fillBox(int row, int col);
int unUsedInBox(int rowStart, int colStart, int num);
int randomGenerator(int num);
void removeKDigits();
int CheckIfSafe(int i, int j, int num);
int unUsedInRow(int i, int num);
int unUsedInCol(int j, int num);
int fillRemaining(int i, int j);

void printSudoku();

// Global Sudoku puzzle array for Solver
int arr[N][N];
// Global Sudoku puzzle array for Generator
int mat[N][N];

int main(int argc, char **argv) {
    int choice;
    printf("Choose an option:\n");
    printf("1. Solve a Sudoku puzzle\n");
    printf("2. Generate a Sudoku puzzle\n");
    printf("Enter your choice (1 or 2): ");
    scanf("%d", &choice);
    getchar(); // Consume the newline character after scanf

    if (choice == 1) {
        // Sudoku Solver
        do {
            printf("Enter Sudoku Puzzle (9 rows of 9 numbers, separated by spaces):\n");
            readSudoku();
        } while (!validateSudoku());

        printf("\nOriginal Sudoku Puzzle:\n");
        printMatrix();

        if (solve_sudoku()) {
            printf("\nSolved Sudoku Puzzle:\n");
            printMatrix();
        } else {
            printf("\nNo solution exists for the given Sudoku puzzle\n");
        }
    } else if (choice == 2) {
        // Sudoku Generator
        srand(time(0)); // Seed the random number generator

        memset(mat, 0, sizeof(mat)); // Initialize the Sudoku matrix with zeros
        fillDiagonal(); // Fill diagonal boxes
        fillRemaining(0, SRN); // Fill remaining cells
        removeKDigits(); // Remove K digits to create the puzzle

        printf("\nGenerated Sudoku Puzzle:\n");
        printSudoku(); // Print the final Sudoku puzzle
    } else {
        printf("Invalid choice.\n");
    }

    return 0;
}

// Function to validate user inputted Sudoku row
int readAndValidateRow(int row) {
    printf("Row %d: ", row + 1);
    char line[N * 3]; // Buffer to handle extra characters
    int num, count = 0;
    char* context; // Additional parameter for strtok_s

    if (fgets(line, sizeof(line), stdin)) {
        // Check if more than 9 numbers are entered
        int extraNumbers = 0;
        for (int i = 0; line[i] != '\0'; ++i) {
            if (line[i] == ' ') {
                extraNumbers++;
            }
        }
        if (extraNumbers >= N) {
            printf("Too many numbers. Please enter exactly 9 numbers.\n");
            return 0; // Too many numbers
        }

        char* token = strtok_s(line, " \n", &context);
        while (token != NULL && count < N) {
            if (sscanf(token, "%d", &num) == 1 && num >= 0 && num <= 9) {
                arr[row][count++] = num;
            } else {
                printf("Invalid input. Please enter numbers between 0 and 9.\n");
                return 0; // Invalid input
            }
            token = strtok_s(NULL, " \n", &context);
        }

        if (count != N) {
            printf("Incomplete row. Please enter exactly 9 numbers.\n");
            return 0; // Incomplete row
        }
        return 1; // Valid input
    } else {
        printf("Invalid input. Please enter 9 numbers separated by spaces.\n");
        return 0; // Invalid input
    }
}

// Function to read Sudoku puzzle from user input
void readSudoku() {
    for (int i = 0; i < N; i++) {
        while (!readAndValidateRow(i)) {
            // Keep asking for the row until a valid input is given
        }
    }
}

// Function to check if the inserted element is safe
int is_safe(int x, int y) {
    int key = arr[x][y];
    for (int i = 0; i < N; i++) {
        // Check the row and column
        if ((i != y && arr[x][i] == key) || (i != x && arr[i][y] == key)) {
            return 0;
        }
    }

    // Check the 3x3 subgrid
    int r_start = (x / 3) * 3;
    int c_start = (y / 3) * 3;
    for (int i = r_start; i < r_start + 3; i++) {
        for (int j = c_start; j < c_start + 3; j++) {
            if (i != x && j != y && arr[i][j] == key) {
                return 0;
            }
        }
    }

    return 1;
}

// Function to solve Sudoku using Backtracking
int solve_sudoku() {
    int row = -1;
    int col = -1;
    int isEmpty = 1;
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            if (arr[i][j] == 0) {
                row = i;
                col = j;

                // We still have some remaining missing values in Sudoku
                isEmpty = 0;
                break;
            }
        }
        if (!isEmpty) {
            break;
        }
    }

    // No empty space left
    if (isEmpty) {
        return 1;
    }

    // Else for each number 1 to 9, backtrack
    for (int num = 1; num <= N; num++) {
        arr[row][col] = num; // Assign the number to see if it fits
        if (is_safe(row, col)) {
            if (solve_sudoku()) { // Recursively solve for the next cells
                return 1;
            }
        }
        arr[row][col] = 0; // Reset the cell as the number doesn't fit
    }
    return 0;
}

// Function to validate the Sudoku puzzle
int validateSudoku() {
    int seen[N + 1];

    // Check each row
    for (int i = 0; i < N; i++) {
        memset(seen, 0, sizeof(seen));
        for (int j = 0; j < N; j++) {
            if (arr[i][j] != 0) {
                if (seen[arr[i][j]]) {
					printf("\nOriginal Sudoku Puzzle:\n");
					printMatrix();
                    printf("\nRow %d violates the Sudoku rules.\n\n", i + 1);
                    return 0;
                }
                seen[arr[i][j]] = 1;
            }
        }
    }

    // Check each column
    for (int j = 0; j < N; j++) {
        memset(seen, 0, sizeof(seen));
        for (int i = 0; i < N; i++) {
            if (arr[i][j] != 0) {
                if (seen[arr[i][j]]) {
					printf("\nOriginal Sudoku Puzzle:\n");
					printMatrix();
                    printf("\nColumn %d violates the Sudoku rules.\n\n", j + 1);
                    return 0;
                }
                seen[arr[i][j]] = 1;
            }
        }
    }

    // Check each 3x3 sub-matrix
    for (int block = 0; block < N; block++) {
        int rowStart = (block / 3) * 3;
        int colStart = (block % 3) * 3;
        memset(seen, 0, sizeof(seen));
        for (int i = rowStart; i < rowStart + 3; i++) {
            for (int j = colStart; j < colStart + 3; j++) {
                if (arr[i][j] != 0) {
                    if (seen[arr[i][j]]) {					
						printf("\nOriginal Sudoku Puzzle:\n");
						printMatrix();
                        printf("\n3x3 Block starting at row %d, column %d violates the Sudoku rules.\n\n", rowStart + 1, colStart + 1);
                        return 0;
                    }
                    seen[arr[i][j]] = 1;
                }
            }
        }
    }

    return 1;
}

// Function to print the Sudoku from Solver
void printMatrix() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", arr[i][j]);
        }
        printf("\n");
    }
}

void fillDiagonal() {
    for (int i = 0; i < N; i += SRN) {
        fillBox(i, i); // Fill each 3x3 diagonal box
    }
}

void fillBox(int row, int col) {
    int num;
    for (int i = 0; i < SRN; i++) {
        for (int j = 0; j < SRN; j++) {
            do {
                num = randomGenerator(N);  // Generate a random number
            } while (!unUsedInBox(row, col, num)); // Ensure the number is not already used in the box
            mat[row + i][col + j] = num; // Place the number in the box
        }
    }
}

int unUsedInBox(int rowStart, int colStart, int num) {
    for (int i = 0; i < SRN; i++) {
        for (int j = 0; j < SRN; j++) {
            if (mat[rowStart + i][colStart + j] == num) {
                return 0; // Number already used in the box
            }
        }
    }
    return 1; // Number not used in the box
}

int randomGenerator(int num) {
    return (rand() % num) + 1; // Generate a random number between 1 and num
}

void removeKDigits() {
    int count = K; 
    while (count != 0) {
        int cellId = randomGenerator(N * N) - 1; // Pick a random cell
        int i = (cellId / N); // Calculate row index
        int j = cellId % N;   // Calculate column index
        if (mat[i][j] != 0) {
            count--;
            mat[i][j] = 0; // Remove the digit from the cell
        }
    }
}

int CheckIfSafe(int i, int j, int num) {
	// Check if it's safe to place num in the cell at (i, j)
    return (unUsedInRow(i, num) && unUsedInCol(j, num) && unUsedInBox(i - i % SRN, j - j % SRN, num));
}

int unUsedInRow(int i, int num) {
    for (int j = 0; j < N; j++) {
        if (mat[i][j] == num) {
            return 0; // Number already used in the row
        }
    }
    return 1; // Number not used in the row
}

int unUsedInCol(int j, int num) {
    for (int i = 0; i < N; i++) {
        if (mat[i][j] == num) {
            return 0; // Number already used in the column
        }
    }
    return 1; // Number not used in the column
}

int fillRemaining(int i, int j) {
    // If we have reached the end of the column, and there are still rows left,
    // move to the next row and start from the first column
    if (j >= N && i < N - 1) {
        i = i + 1;
        j = 0;
    }

    // If we have filled all rows and columns, return true indicating success
    if (i >= N && j >= N) {
        return 1;
    }

    // If we are in the first 3 rows (i < SRN) and we reach the diagonal 3x3 box,
    // skip to the next set of columns after the diagonal box
    if (i < SRN) {
        if (j < SRN) {
            j = SRN;
        }
    }
    // If we are in the last 3 rows (i >= N - SRN) and reach the diagonal 3x3 box,
    // move to the next row and start from the first column
    else if (i < N - SRN) {
        if (j == (i / SRN) * SRN) {
            j = j + SRN;
        }
    }
    // For the last few rows, skip the diagonal 3x3 box if reached
    else {
        if (j == N - SRN) {
            i = i + 1;
            j = 0;
            if (i >= N) {
                return 1;
            }
        }
    }

    // Try placing numbers from 1 to N in the current cell
    for (int num = 1; num <= N; num++) {
        // Check if it's safe to place the number in the current cell
        if (CheckIfSafe(i, j, num)) {
            mat[i][j] = num; // Place the number

            // Recursively call fillRemaining for the next cell
            if (fillRemaining(i, j + 1)) {
                return 1;
            }

            // If placing the current number doesn't lead to a solution,
            // reset the cell and try the next number
            mat[i][j] = 0;
        }
    }

    // Return false if no number can be placed in this cell
    return 0;
}

// Function to print Sudoku from Generator
void printSudoku() {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            printf("%d ", mat[i][j]); // Print each number in the Sudoku grid
        }
        printf("\n"); // New line after each row
    }
    printf("\n"); // New line after the grid
}
