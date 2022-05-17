#include <stdio.h>
#include <stdlib.h>
#include <omp.h>
#include <string.h>

void print_result(const size_t* histogram, const size_t bin_size) {
    /* Output the results. */
    size_t sum = 0;
    for (int i = 0; i < bin_size; ++i) {
        printf("bin[%d]=%zu\n", i, histogram[i]);
        sum += histogram[i];
    }
    printf("%zu\n", sum);
}

double thread_bin_version(const double* nums, const size_t random_num, const size_t nums_bin) {
    size_t bin_size = (size_t)(100.0 / (double) nums_bin);
    size_t* histogram = (size_t*)malloc(bin_size * sizeof(size_t));
    memset(histogram, 0, bin_size * sizeof(size_t));

    double start = omp_get_wtime();
#pragma omp parallel for
    for (size_t i = 0; i < bin_size; ++i) {
        for (size_t j = 0; j < random_num; ++j) {
            size_t bin = (size_t)(nums[j] / (double)nums_bin);
            if (bin == i) {
                histogram[bin] += 1;
            }
        }
    }
    double end = omp_get_wtime();
    print_result(histogram, bin_size);
    free(histogram);
    return end - start;
}

int main(int argc, char* argv[]) {
    size_t nums_bin = 0;
    size_t nums_thread = 0;
    char* file_name = (char*) malloc(256);

    if (argc < 4) {
        fprintf(stderr, "Please enter 3 arguments:\n"
                        "b: the number of bins, 0 < b <= 50\n"
                        "t: number of threads, 0 < t <= 100\n"
                        "filename: the name of the text file that contains the floating point numbers\n");
        exit(1);
    }

    /* Default settings. */
    nums_bin = strtol(argv[1], NULL, 10);
    nums_thread = strtol(argv[2], NULL, 10);
    strcpy(file_name, argv[3]);
    omp_set_num_threads((int )nums_thread);

    /* Reading the files. */
    FILE* file;
    file = fopen(file_name, "r");
    char buffer[128];
    if (!file) {
        fprintf(stderr, "Fail to open %s\n", file_name);
    }
    fscanf(file, "%s", buffer);
    size_t random_num = strtol(buffer, NULL, 10);
    double* nums = (double *) malloc(random_num * sizeof(double));
    int idx = 0;
    // char* => double
    while (fscanf(file, "%s", buffer) != EOF) {
        nums[idx++] = strtod(buffer, NULL);
    }
    fclose(file);

    /* 2. Each thread is responsible for one bin. */
    printf("2.Bin Version:\n");
    double bin_time = thread_bin_version(nums, random_num, nums_bin);
    printf("Time: %f\n\n", bin_time);

    free(file_name);
    free(nums);
    return 0;
}