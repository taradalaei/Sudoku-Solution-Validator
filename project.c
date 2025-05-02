#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define SIZE 9

int sudoku[SIZE][SIZE] = {
    {6,2,4,5,3,9,1,8,7},
    {5,1,9,7,2,8,6,3,4},
    {8,3,7,6,1,4,2,9,5},
    {1,4,3,8,6,5,7,2,9},
    {9,5,8,2,4,7,3,6,1},
    {7,6,2,3,9,1,4,5,8},
    {3,7,1,9,5,6,8,4,2},
    {4,9,6,1,8,2,5,7,3},
    {2,8,5,4,7,3,9,1,6}
};

int valid[3*SIZE]; // 9 rows + 9 columns + 9 boxes

typedef struct {
    int row;
    int col;
} parameters;

void *check_row(void *param) {
    parameters *p = (parameters *) param;
    int row = p->row;
    int seen[SIZE+1] = {0};
    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[row][i];
        if (num < 1 || num > 9 || seen[num]) {
            pthread_exit(NULL);
        }
        seen[num] = 1;
    }
    valid[row] = 1;
    pthread_exit(NULL);
}

void *check_col(void *param) {
    parameters *p = (parameters *) param;
    int col = p->col;
    int seen[SIZE+1] = {0};
    for (int i = 0; i < SIZE; i++) {
        int num = sudoku[i][col];
        if (num < 1 || num > 9 || seen[num]) {
            pthread_exit(NULL);
        }
        seen[num] = 1;
    }
    valid[9 + col] = 1;
    pthread_exit(NULL);
}

void *check_box(void *param) {
    parameters *p = (parameters *) param;
    int row = p->row;
    int col = p->col;
    int seen[SIZE+1] = {0};
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            int num = sudoku[row + i][col + j];
            if (num < 1 || num > 9 || seen[num]) {
                pthread_exit(NULL);
            }
            seen[num] = 1;
        }
    }
    int boxIndex = 18 + (row/3)*3 + (col/3);
    valid[boxIndex] = 1;
    pthread_exit(NULL);
}

int main() {
    pthread_t threads[27];
    int t = 0;

    // Create threads for rows and columns
    for (int i = 0; i < SIZE; i++) {
        parameters *data_row = (parameters *) malloc(sizeof(parameters));
        data_row->row = i;
        data_row->col = 0;
        pthread_create(&threads[t++], NULL, check_row, data_row);

        parameters *data_col = (parameters *) malloc(sizeof(parameters));
        data_col->row = 0;
        data_col->col = i;
        pthread_create(&threads[t++], NULL, check_col, data_col);
    }

    // Create threads for 3x3 boxes
    for (int i = 0; i < SIZE; i += 3) {
        for (int j = 0; j < SIZE; j += 3) {
            parameters *data_box = (parameters *) malloc(sizeof(parameters));
            data_box->row = i;
            data_box->col = j;
            pthread_create(&threads[t++], NULL, check_box, data_box);
        }
    }

    // Wait for all threads to complete
    for (int i = 0; i < 27; i++) {
        pthread_join(threads[i], NULL);
    }

    // Check final result
    int isValid = 1;
    for (int i = 0; i < 27; i++) {
        if (valid[i] == 0) {
            isValid = 0;
            break;
        }
    }


    if (isValid)
        printf("Sudoku is validated.\n");
    else
        printf("Sudoku is not validated.\n");

    return 0;
}
