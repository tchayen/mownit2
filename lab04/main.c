#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <math.h>
#include <gsl/gsl_blas.h>
#include <gsl/gsl_vector.h>
#include <gsl/gsl_matrix.h>

double** create_matrix(int size) {
    double** matrix = calloc(size, sizeof(double*));
    for (int i = 0; i < size; i++)
        matrix[i] = calloc(size, sizeof(double));
    return matrix;
}

double rand_in(double min, double max) {
    double range = (max - min);
    double div = RAND_MAX / range;
    return min + (rand() / div);
}

void clear_matrix(double** matrix, int size) {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i][j] = 0.0;
}

void free_matrix(double** matrix, int size) {
    for (int i = 0; i < size; i++)
        free(matrix[i]);
    free(matrix);
}

double** fill_matrix(double** matrix, int size) {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            matrix[i][j] = rand_in(1.0, 100.0);
    return matrix;
}

gsl_matrix* get_random_gsl_matrix(int size) {
    gsl_matrix* matrix = gsl_matrix_alloc(size, size);
    for (int i = 0; i < size * size; i++) {
        double value = rand_in(1.0, 100.0);
        gsl_matrix_set(matrix, i / size, i % size, value);
    }
    return matrix;
}

double subtract_time(struct timeval start, struct timeval end) {
    double ssec = ((double) start.tv_sec)  + (((double) start.tv_usec) / 1000000);
    double esec = ((double) end.tv_sec)  + (((double) end.tv_usec) / 1000000);
    return esec - ssec;
}

void write_to_file(FILE* file, int size, char* method, double timesec) {
    fprintf(file,"%d, %s, %lf\n", size, method, timesec);
}

void naive_multiplication(double** A, double** B, double** C, int size) {
    for (int i = 0; i < size; i++)
        for (int j = 0; j < size; j++)
            for (int k = 0; k < size; k++)
                C[i][j] += A[i][k] * B[k][j];
}

void better_multiplication(double** A, double** B, double** C, int size) {
    for(int i = 0; i < size; i++)
        for(int k = 0; k < size; k++)
            for(int j = 0; j < size; j++)
                C[i][j] += A[i][k] * B[k][j];
}

/* -------------------------------------------------------------------------- */
int main() {
    FILE* fp = fopen("c_measurements.csv", "w+");
    struct timeval start, end;

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            int s = (j + 1) * 100;
            double** A = fill_matrix(create_matrix(s), s);
            double** B = fill_matrix(create_matrix(s), s);
            double** C = create_matrix(s);

            /* naive multiplication */
            gettimeofday(&start, NULL);
            naive_multiplication(A, B, C, s);
            gettimeofday(&end, NULL);
            write_to_file(fp, s, "naive", subtract_time(start, end));
            printf("%d, %s, %lf\n", s, "naive", subtract_time(start, end));
            clear_matrix(C, s);

            /* better multiplication */
            gettimeofday(&start, NULL);
            better_multiplication(A, B, C, s);
            gettimeofday(&end, NULL);
            write_to_file(fp, s, "better", subtract_time(start, end));
            printf("%d, %s, %lf\n", s, "better", subtract_time(start, end));

            free_matrix(A, s);
            free_matrix(B, s);
            free_matrix(C, s);
        }
    }

    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 10; j++) {
            int s = (j + 1) * 100;
            gsl_matrix* A = get_random_gsl_matrix(s);
            gsl_matrix* B = get_random_gsl_matrix(s);
            gsl_matrix* C = gsl_matrix_alloc(s, s);

            gettimeofday(&start, NULL);
            gsl_blas_dgemm(CblasNoTrans, CblasNoTrans, 1.0, A, B, 0.0, C);
            gettimeofday(&end, NULL);
            write_to_file(fp, s, "gsl-blas", subtract_time(start, end));
            printf("%d, %s, %lf\n", s, "gsl-blas", subtract_time(start, end));

            gsl_matrix_free(C);
            gsl_matrix_free(A);
            gsl_matrix_free(B);
        }
    }
    return 0;
}
