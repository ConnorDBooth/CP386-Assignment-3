#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <stdbool.h>



typedef struct{
    int row;
    int column;
} parameters; 

#define SIZE 9

int sudoku[SIZE][SIZE];

int row_valid[SIZE] = {0};
int column_valid[SIZE] = {0};
int subgrid_valid[SIZE] = {0}; 


int check_valid(int *array){
    int digits[SIZE + 1] = {0};

    for (int i = 0; i < SIZE; i++){
        int num = array[i];

        if (num < 1 || num > 9 || digits[num] == 1){
            return 0;
        }

        digits[num] = 1; 
    }

    return 1;
}

void *check_row(void *param){
    for (int i = 0; i < SIZE; i++){
        if (!check_valid(sudoku[i])){
            row_valid[i] = 0;
            pthread_exit(NULL);
        }
        row_valid[i] = 1;
    }
    pthread_exit(NULL);
}

void *check_column(void *param){
    for (int i = 0; i < SIZE; i++){
        int column[SIZE];
        for (int j = 0; j < SIZE; j++){
            column[j] = sudoku[j][i];
        }
        if (!check_valid(column)){
            column_valid[i] = 0;
            pthread_exit(NULL);
        }
        column_valid[i] = 1;
    }
    pthread_exit(NULL);

}

void *check_subgrid(void *param){
    parameters *data = (parameters *) param; 
    int startRow = data->row;
    int startCol = data->column;
    int subgrid[SIZE];
    int index = 0;

    for (int i = startRow; i < startRow + 3; i++){
        for (int j = startCol; j < startCol + 3; j++){
            subgrid[index++] = sudoku[i][j];
        }
    }

    int gird_num = (startRow / 3) * 3 + (startCol / 3);
    subgrid_valid[gird_num] = check_valid(subgrid);

    free(data);
    pthread_exit(NULL);
}
void read_sudoku_from_file(const char *file_name){
    FILE *file = fopen(file_name, "r");
    if (file == NULL){
        perror("Error opening file");
        exit(EXIT_FAILURE);
    }

    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            fscanf(file, "%d", &sudoku[i][j]);
        }
    }
    fclose(file);
}

void print_puzzle(){
    printf("Sudoku Puzzle Solution is:\n");
    for (int i = 0; i < SIZE; i++){
        for (int j = 0; j < SIZE; j++){
            printf("%d ", sudoku[i][j]);
        }
        printf("\n");
    }
}

int main (int argc, char* argv[]){
    read_sudoku_from_file("sample_in_sudoku.txt");

    pthread_t row_thread, col_thread, subgrid_threads[SIZE];

    pthread_create(&row_thread, NULL, check_row, NULL);
    pthread_create(&col_thread, NULL, check_column, NULL);

    for (int i = 0; i < SIZE; i+=3){
        for (int j = 0; j < SIZE; j+=3){
            parameters *data = (parameters *) malloc(sizeof(parameters));
            data->row = i;
            data->column = j;
            pthread_create(&subgrid_threads[(i / 3) * 3 + (j / 3)], NULL, check_subgrid, data);
        }
    }

    pthread_join(row_thread, NULL);
    pthread_join(col_thread, NULL);

    for(int i = 0; i < SIZE; i++){
        pthread_join(subgrid_threads[i], NULL);
    }

    int valid = 1;
    for (int i = 0; i < SIZE; i++){
        if (!row_valid[i] || !column_valid[i] || !subgrid_valid[i]){
            valid = 0;
            break;
        }
    }

    print_puzzle();

    if (valid){
        printf("Sudoku puzzle is valid\n");
    } else {
        printf("Sudoku puzzle is invalid\n");
    }

    return 0; 
}