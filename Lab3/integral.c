/* 
Argumenty:
 1) liczba wątków
 2) początek przedziału a
 3) koniec przedziału b
 */

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>


typedef struct {
    double a;
    double b;
    double result;
} ThreadData;


void *thread_func(void *arg) {
    ThreadData *data = (ThreadData *)arg;

 
    data->result = (data->a + data->b) / 2.0; //przykladowe obliczenia

    printf("Watek: przedzial [%.2f, %.2f] -> wynik = %.4f\n",
           data->a, data->b, data->result);

    return NULL;
}

int main(int argc, char *argv[]) {
    if (argc < 4) {
        fprintf(stderr, "Uzycie: %s liczba_watkow a b\n", argv[0]);
        return EXIT_FAILURE;
    }

    int num_threads = atoi(argv[1]);
    double a = atof(argv[2]);
    double b = atof(argv[3]);

    if (num_threads < 1) {
        fprintf(stderr, "Liczba wątków musi być >= 1\n");
        return EXIT_FAILURE;
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    ThreadData *args = malloc(sizeof(ThreadData) * num_threads);
    if (!threads || !args) {
        perror("malloc");
        return EXIT_FAILURE;
    }

    double step = (b - a) / num_threads;

    for (int i = 0; i < num_threads; ++i) {
        args[i].a = a + i * step;
        args[i].b = (i == num_threads - 1) ? b : a + (i + 1) * step;
        args[i].result = 0.0;

        int rc = pthread_create(&threads[i], NULL, thread_func, &args[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create nie powiodl sie: %s\n", strerror(rc));
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    double sum = 0.0;
    for (int i = 0; i < num_threads; ++i) {
        sum += args[i].result;
    }

    printf("Suma wynikow ze wszystkich watkow = %.4f\n", sum);

    free(threads);
    free(args);
    return EXIT_SUCCESS;
}
