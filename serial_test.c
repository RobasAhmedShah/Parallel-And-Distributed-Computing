#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#define ARRAY_SIZE 100000 // also run for 500K, 1M, 2M and 5M
int main()
{
    double *array = (double *)malloc(ARRAY_SIZE * sizeof(double));
    if (array == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = (double)rand() / RAND_MAX;
    }
    clock_t start_time = clock();
    for (int i = 0; i < ARRAY_SIZE; i++)
    {
        array[i] = sin(array[i]) * cos(array[i]) + tan(array[i]);
        array[i] = log(array[i] + 1.0) * sqrt(array[i] + 2.0);
        array[i] = pow(array[i], 2.0) - exp(array[i]);
    }
    clock_t end_time = clock();
    double elapsed_time = (double)(end_time - start_time) / CLOCKS_PER_SEC;
    printf("Execution time: %f seconds\n", elapsed_time);
    free(array);
    return 0;
}