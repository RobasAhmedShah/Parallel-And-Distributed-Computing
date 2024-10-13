#include <stdio.h>
#include <stdlib.h>
#include <omp.h>

#define SMALL_MATRIX_THRESHOLD 100  // Threshold for small matrices
#define LARGE_MATRIX_THRESHOLD 1000 // Threshold for large matrices

// Function to initialize a matrix with random values
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

// Function to allocate a matrix dynamically
double **allocate_matrix(int rows, int cols)
{
    double **matrix = (double **)malloc(rows * sizeof(double *));
    for (int i = 0; i < rows; i++)
    {
        matrix[i] = (double *)malloc(cols * sizeof(double));
    }
    return matrix;
}

// Function to free dynamically allocated matrix
void free_matrix(double **matrix, int rows)
{
    for (int i = 0; i < rows; i++)
    {
        free(matrix[i]);
    }
    free(matrix);
}

int main()
{
    // Modify these values to change the matrix size
    int rows_A = 500, cols_A = 500, rows_B = 500, cols_B = 500;

    // Check if matrix dimensions are valid for multiplication
    if (cols_A != rows_B)
    {
        printf("Matrix multiplication is not possible with these dimensions\n");
        return 1;
    }

    // Dynamically allocate matrices A, B, and C
    double **A = allocate_matrix(rows_A, cols_A);
    double **B = allocate_matrix(rows_B, cols_B);
    double **C = allocate_matrix(rows_A, cols_B);

    // Initialize matrices A and B with random values
    initialize_matrix(A, rows_A, cols_A);
    initialize_matrix(B, rows_B, cols_B);

    // Adjust number of threads based on matrix size
    int num_threads;
    int matrix_size = rows_A * cols_A;
    if (matrix_size < SMALL_MATRIX_THRESHOLD * SMALL_MATRIX_THRESHOLD)
    {
        num_threads = 2; // Fewer threads for small matrices
    }
    else if (matrix_size > LARGE_MATRIX_THRESHOLD * LARGE_MATRIX_THRESHOLD)
    {
        num_threads = 8; // More threads for large matrices
    }
    else
    {
        num_threads = 4; // Default to 4 threads for medium-sized matrices
    }

    omp_set_num_threads(num_threads);

    // Matrix multiplication
    double start_time = omp_get_wtime(); // Start timing

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

    double end_time = omp_get_wtime(); // End timing

    // Output execution time and number of threads used
    printf("Matrix multiplication completed.\n");
    printf("Execution time: %f seconds\n", end_time - start_time);
    printf("Number of threads used: %d\n", num_threads);

    // Free matrices
    free_matrix(A, rows_A);
    free_matrix(B, rows_B);
    free_matrix(C, rows_A);

    return 0;
}
