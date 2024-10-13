#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        printf("Usage: %s <array_size>\n", argv[0]);
        return 1;
    }

    int array_size = atoi(argv[1]);
    double *array = (double *)malloc(array_size * sizeof(double));
    if (array == NULL)
    {
        printf("Memory allocation failed\n");
        return 1;
    }

    for (int i = 0; i < array_size; i++)
    {
        array[i] = (double)rand() / RAND_MAX;
    }

    clock_t start_time = clock();

    for (int i = 0; i < array_size; i++)
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
