#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <omp.h>

#define SMALL_ARRAY_THRESHOLD 10000   // Threshold for small arrays
#define LARGE_ARRAY_THRESHOLD 1000000 // Threshold for large arrays
#define ARRAY_SIZE 100000             // Modify this to test different sizes

int main()
{
    double *array = (double *)malloc(ARRAY_SIZE * sizeof(double));
    if (array == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    // Adjust number of threads based on the size of the array
    int num_threads;
    if (ARRAY_SIZE < SMALL_ARRAY_THRESHOLD)
    {
        num_threads = 2; // Fewer threads for small arrays
    }
    else if (ARRAY_SIZE > LARGE_ARRAY_THRESHOLD)
    {
        num_threads = 8; // More threads for large arrays
    }
    else
    {
        num_threads = 4; // Default to 4 threads for medium-sized arrays
    }

    // Set number of threads dynamically
    omp_set_num_threads(num_threads);

// Initialize array with random values (parallelized)
#pragma omp parallel for shared(array) schedule(static)
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = (double)rand() / RAND_MAX;
    }

    // Perform math operations to simulate CPU load
    double start_time = omp_get_wtime(); // Using OpenMP for high-resolution timing

// Use dynamic scheduling if the array is large for better load balancing
#pragma omp parallel for shared(array) schedule(dynamic)
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = sin(array[i]) * cos(array[i]) + tan(array[i]);
        array[i] = log(array[i] + 1.0) * sqrt(array[i] + 2.0);
        array[i] = pow(array[i], 2.0) - exp(array[i]);
    }

    double end_time = omp_get_wtime();
    double elapsed_time = end_time - start_time;

    // Output execution time and number of threads used
    printf("Execution time: %f seconds\n", elapsed_time);
    printf("Number of threads used: %d\n", num_threads);

    free(array);
    return 0;
}