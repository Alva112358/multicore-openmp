#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <omp.h>

int threads_num; // number of threads.
int N;           // prime number.

/**
 * @brief Set the configuration object.
 *
 * @param threads the number of threads in the OpenMP configuration.
 */
void set_configuration(int threads) {
    omp_set_num_threads(threads);
}

/**
 * @brief write results to text file.
 *
 * @param primes
 */
void write_to_text_file(const int* primes) {
    FILE* fp = NULL;
    char name[20];
    sprintf(name, "%d.txt", N);
    fp = fopen(name, "w");
    int i = 0;
    int count = 1;

    for (i = 2; i <= N; ++i) {
        if (primes[i] == 1) {
            fprintf(fp, "%d,%d\n", count++, i);
        }
    }
    fclose(fp);
}

/**
 * @brief Check the validation of arguments.
 *
 * @param argc number of arguments.
 * @param argv content of arguments.
 */
void check_args(int argc, char* argv[]) {
    if (argc < 3) {
        fprintf(stderr, "Usage: <exec_filename> <N> <t>\n"
                        "  <N>: a positive number greater than 2\n"
                        "  <t>: the number of threads and is positive integer that does not exceed 100.\n");
        exit(1);
    }

    N = (int)strtol(argv[1], NULL, 10);
    if (N < 2) {
        fprintf(stderr, "N could not smaller than 2.\n");
        exit(1);
    }

    threads_num = (int)strtol(argv[2], NULL, 10);
    if (threads_num <= 0 || threads_num > 100) {
        fprintf(stderr, "Thread number could not exceed 100.\n");
        exit(1);
    }
}

/**
 * @brief Parallel version prime number will be marked as 1, else 0.
 * @param threads the number of threads to run the program.
 * @return prime number
 */
int* generate_prime_parallel() {
    size_t i;
    int* primes = (int*)malloc((N + 1) * sizeof(int));
    int upper = (double)(N + 1) / 2;
    primes[0] = primes[1] = 0;
#pragma omp parallel for
    for (i = 2; i <= N; ++i) {
        primes[i] = 1;
    }

#pragma omp parallel for schedule(dynamic, 8)
    for (i = 2; i <= upper; ++i) {
        if (primes[i] == 1) {
            size_t j;
            for (j = i * i; j <= N; j += i) {
                primes[j] = 0;
            }
        }
    }

    return primes;
}

int main(int argc, char* argv[]) {
    check_args(argc, argv);

    printf("%lu\n", 64 / sizeof(uint64_t));

    omp_set_num_threads(1);
    double tstart = omp_get_wtime();
    int* primes = generate_prime_parallel();
    double ttaken = omp_get_wtime() - tstart;
    printf("Time take for the main part: %f\n", ttaken);

    omp_set_num_threads(threads_num);
    double tstart2 = omp_get_wtime();
    int* primes2 = generate_prime_parallel();
    double ttaken2 = omp_get_wtime() - tstart2;
    printf("Time take for the main part: %f\n", ttaken2);

    write_to_text_file(primes2);
    free(primes2);

    return 0;
}