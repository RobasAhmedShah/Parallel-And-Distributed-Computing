#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

void initialize_matrix(double **matrix, int rows, int cols)
{
#pragma omp parallel for
    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            matrix[i][j] = (double)rand() / RAND_MAX;
        }
    }
}

double **allocate_matrix(int rows, int cols)
{
    double **matrix = (double **)malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++)
    {
        matrix[i] = (double *)malloc(cols * sizeof(double));
    }
    return matrix;
}

void free_matrix(double **matrix, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }

    int size = atoi(argv[1]);
    int rows_A = size, cols_A = size, rows_B = size, cols_B = size;

    if (cols_A != rows_B)
    {
        printf("Matrix multiplication is not possible with these dimensions\n");
        return 1;
    }

    double **A = allocate_matrix(rows_A, cols_A);
    double **B = allocate_matrix(rows_B, cols_B);
    double **C = allocate_matrix(rows_A, cols_B);

    initialize_matrix(A, rows_A, cols_A);
    initialize_matrix(B, rows_B, cols_B);

    omp_set_num_threads(4);

    double start_time = omp_get_wtime();

#pragma omp parallel for shared(A, B, C) schedule(dynamic)
    for (int i = 0; i < rows_A; i++)
    {
        for (int j = 0; j < cols_B; j++)
        {
            C[i][j] = 0.0;
            for (int k = 0; k < cols_A; k++)
            {
                C[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    double end_time = omp_get_wtime();

    printf("Matrix multiplication completed.\n");
    printf("Execution time: %f seconds\n", end_time - start_time);
    printf("Number of threads used: 4\n");

    free_matrix(A, rows_A);
    free_matrix(B, rows_B);
    free_matrix(C, rows_A);

    return 0;
}
