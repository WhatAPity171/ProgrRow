#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <errno.h>
#include <string.h>

void *thread_func(void *arg) {
    int thread_id = *(int *)arg;                 
    pthread_t self = pthread_self();            

    printf("Watek przeslany_id=%d  pthread_self()=%lu\n",
           thread_id, (unsigned long)self);


    return NULL;
}

int main(int argc, char *argv[]) {
    int num_threads = 4; // domyślna liczba wątków

    if (argc >= 2) {
        char *endptr;
        long v = strtol(argv[1], &endptr, 10);
        if (*endptr == '\0' && v > 0) {
            num_threads = (int)v;
        } else {
            fprintf(stderr, "Niepoprawny argument, uzycie: %s [liczba_watkow]\n", argv[0]);
            return EXIT_FAILURE;
        }
    }

    pthread_t *threads = malloc(sizeof(pthread_t) * num_threads);
    if (!threads) {
        perror("malloc threads");
        return EXIT_FAILURE;
    }

    int *ids = malloc(sizeof(int) * num_threads); 
    if (!ids) {
        perror("malloc ids");
        free(threads);
        return EXIT_FAILURE;
    }


    for (int i = 0; i < num_threads; ++i) {
        ids[i] = i; 
        int rc = pthread_create(&threads[i], NULL, thread_func, &ids[i]);
        if (rc != 0) {
            fprintf(stderr, "pthread_create dla watku %d nie powiodl sie: %s\n", i, strerror(rc));
            for (int j = 0; j < i; ++j) pthread_join(threads[j], NULL);
            free(threads);
            free(ids);
            return EXIT_FAILURE;
        }
    }

    for (int i = 0; i < num_threads; ++i) {
        int rc = pthread_join(threads[i], NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_join dla watku %d nie powiodl sie: %s\n", i, strerror(rc));
        }
    }

    free(threads);
    free(ids);

    printf("Wszystkie watki zakonczone.\n");
    return EXIT_SUCCESS;
}
