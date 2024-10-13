#include <stdio.h>
#include <stdlib.h>
#include <mpi.h>

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
    if (argc != 2)
    {
        printf("Usage: %s <matrix_size>\n", argv[0]);
        return 1;
    }

    int matrix_size = atoi(argv[1]);

    int rank, size;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);

    int rows_per_proc = matrix_size / size;
    int remainder = matrix_size % size;
    int local_rows = (rank < remainder) ? rows_per_proc + 1 : rows_per_proc;

    double *A = NULL, *B = NULL, *C = NULL, *local_A = NULL, *local_C = NULL;

    if (rank == 0)
    {
        A = (double *)malloc(matrix_size * matrix_size * sizeof(double));
        B = (double *)malloc(matrix_size * matrix_size * sizeof(double));
        C = (double *)malloc(matrix_size * matrix_size * sizeof(double));
        initialize_matrix(A, matrix_size);
        initialize_matrix(B, matrix_size);
        printf("Top-left corner of matrix A (Rank 0):\n");
        print_matrix_portion(A, matrix_size, matrix_size);
        printf("Top-left corner of matrix B (Rank 0):\n");
        print_matrix_portion(B, matrix_size, matrix_size);
    }

    B = (double *)malloc(matrix_size * matrix_size * sizeof(double));
    local_A = (double *)malloc(local_rows * matrix_size * sizeof(double));
    local_C = (double *)malloc(local_rows * matrix_size * sizeof(double));

    MPI_Bcast(B, matrix_size * matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);
    MPI_Scatter(A, rows_per_proc * matrix_size, MPI_DOUBLE, local_A, local_rows * matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    printf("Rank %d received local_A:\n", rank);
    print_matrix_portion(local_A, matrix_size, local_rows);

    double start_time = MPI_Wtime();

    for (int i = 0; i < local_rows; i++)
    {
        for (int j = 0; j < matrix_size; j++)
        {
            local_C[i * matrix_size + j] = 0.0;
            for (int k = 0; k < matrix_size; k++)
            {
                local_C[i * matrix_size + j] += local_A[i * matrix_size + k] * B[k * matrix_size + j];
            }
        }
    }

    MPI_Gather(local_C, local_rows * matrix_size, MPI_DOUBLE, C, rows_per_proc * matrix_size, MPI_DOUBLE, 0, MPI_COMM_WORLD);

    double end_time = MPI_Wtime();

    if (rank == 0)
    {
        printf("Matrix multiplication completed.\n");
        printf("Execution time: %f seconds\n", end_time - start_time);
        printf("Number of processes: %d\n", size);
        free(A);
        free(C);
    }

    free(B);
    free(local_A);
    free(local_C);

    MPI_Finalize();
    return 0;
}
