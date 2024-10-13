#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

#define MATRIX_SIZE 100

void initialize_matrix(double *matrix, int size)
{
    for (int i = 0; i < size * size; i++)
    {
        matrix[i] = (double)rand() / RAND_MAX;
    }
}

void print_matrix_portion(double *matrix, int size, int rows)
{
    for (int i = 0; i < rows && i < 5; i++)
    {
        for (int j = 0; j < 5 && j < size; j++)
        {
            printf("%f ", matrix[i * size + j]);
        }
        printf("\n");
    }
}

int main(int argc, char **argv)
{
    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_proc = MATRIX_SIZE / size;
    int remainder = MATRIX_SIZE % size;
    int local_rows = (rank < remainder) ? rows_per_proc + 1 : rows_per_proc;

    double *A = NULL, *B = NULL, *C = NULL, *local_A = NULL, *local_C = NULL;

    if (rank == 0)
    {
        A = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        B = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        C = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
        initialize_matrix(A, MATRIX_SIZE);
        initialize_matrix(B, MATRIX_SIZE);

        // Print top-left corner of A to verify
        printf("Top-left corner of matrix A (Rank 0):\n");
        print_matrix_portion(A, MATRIX_SIZE, MATRIX_SIZE);
        
        // Print top-left corner of B to verify
        printf("Top-left corner of matrix B (Rank 0):\n");
        print_matrix_portion(B, MATRIX_SIZE, MATRIX_SIZE);
    }

    B = (double *)malloc(MATRIX_SIZE * MATRIX_SIZE * sizeof(double));
    local_A = (double *)malloc(local_rows * MATRIX_SIZE * sizeof(double));
    local_C = (double *)malloc(local_rows * MATRIX_SIZE * sizeof(double));

    // Broadcast B to all processes
    MPI_Bcast(B, MATRIX_SIZE * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    
    // Scatter A from rank 0 to all processes
    MPI_Scatter(A, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE, local_A, local_rows * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    // Debug output to check the local portion of A received
    printf("Rank %d received local_A:\n", rank);
    print_matrix_portion(local_A, MATRIX_SIZE, local_rows);

    double start_time = MPI_Wtime();

    // Perform local matrix multiplication
    for (int i = 0; i < local_rows; i++)
    {
        for (int j = 0; j < MATRIX_SIZE; j++)
        {
            local_C[i * MATRIX_SIZE + j] = 0.0;
            for (int k = 0; k < MATRIX_SIZE; k++)
            {
                local_C[i * MATRIX_SIZE + j] += local_A[i * MATRIX_SIZE + k] * B[k * MATRIX_SIZE + j];
            }
        }
    }

    // Gather the local results to C on rank 0
    MPI_Gather(local_C, local_rows * MATRIX_SIZE, MPI_DOUBLE, C, rows_per_proc * MATRIX_SIZE, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0)
    {
        printf("Matrix multiplication completed.\n");
        printf("Execution time: %f seconds\n", end_time - start_time);
        printf("Number of processes: %d\n", size);
        printf("Top-left corner of result matrix C:\n");
        print_matrix_portion(C, MATRIX_SIZE, MATRIX_SIZE);

        free(A);
        free(C);
    }

    free(B);
    free(local_A);
    free(local_C);

    MPI_Finalize();
    return 0;
}
