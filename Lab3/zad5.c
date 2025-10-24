

#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <errno.h>
#include <sched.h>
#include <unistd.h>

/* Funkcja wykonywana przez wątek */
void *thread_func(void *arg) {
    (void)arg;

    pthread_attr_t attr;
    int rc = pthread_getattr_np(pthread_self(), &attr);
    if (rc != 0) {
        fprintf(stderr, "pthread_getattr_np error: %s\n", strerror(rc));
        pthread_exit(NULL);
    }

    void *stack_addr;
    size_t stack_size;
    int detach_state;
    int policy;
    struct sched_param param;

    pthread_attr_getstack(&attr, &stack_addr, &stack_size);
    pthread_attr_getdetachstate(&attr, &detach_state);
    pthread_attr_getschedpolicy(&attr, &policy);
    pthread_attr_getschedparam(&attr, &param);

    printf("=== Parametry watku %lu ===\n", (unsigned long)pthread_self());
    printf("  * Adres stosu: %p\n", stack_addr);
    printf("  * Rozmiar stosu: %zu bajtów (%.2f MB)\n", stack_size, stack_size / 1024.0 / 1024.0);
    printf("  * Stan odlaczenia: %s\n", (detach_state == PTHREAD_CREATE_DETACHED) ? "DETACHED" : "JOINABLE");

    const char *policy_name = (policy == SCHED_OTHER) ? "SCHED_OTHER" :
                              (policy == SCHED_RR)    ? "SCHED_RR" :
                              (policy == SCHED_FIFO)  ? "SCHED_FIFO" : "NIEZNANY";
    printf("  * Polityka planowania: %s\n", policy_name);
    printf("  * Priorytet: %d\n", param.sched_priority);

#ifdef __linux__
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    pthread_getaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset);
    printf("  * Rdzenie CPU: ");
    for (int i = 0; i < CPU_SETSIZE; i++) {
        if (CPU_ISSET(i, &cpuset)) printf("%d ", i);
    }
    printf("\n");
#endif

    /* Przykład testu rozmiaru stosu — duża tablica lokalna */
    int tab[1000000] = {0}; // 1 milion elementów ~ 4 MB
    tab[0] = 123;
    tab[999999] = 456;

    printf("  * Test stosu OK (tab[0]=%d, tab[end]=%d)\n", tab[0], tab[999999]);

    pthread_attr_destroy(&attr);
    pthread_exit(NULL);
}

/* Tworzenie wątków z różnymi atrybutami */
int main(void) {
    const int num_threads = 3;
    pthread_t threads[num_threads];
    pthread_attr_t attrs[num_threads];

    size_t stack_sizes[] = {20 * 1024 * 1024, 40 * 1024 * 1024, 80 * 1024 * 1024}; // 2,4,8 MB

    for (int i = 0; i < num_threads; ++i) {
        pthread_attr_init(&attrs[i]);
        pthread_attr_setstacksize(&attrs[i], stack_sizes[i]);

        if (i == 1) { // przykładowy wątek z odłączonym stanem
            pthread_attr_setdetachstate(&attrs[i], PTHREAD_CREATE_DETACHED);
        }

        if (i == 2) { // ustaw inny priorytet, jeśli system pozwala
            struct sched_param param;
            param.sched_priority = 10;
            pthread_attr_setschedpolicy(&attrs[i], SCHED_RR);
            pthread_attr_setschedparam(&attrs[i], &param);
        }

#ifdef __linux__
        if (i == 0) { // przypisz wątek do rdzenia 0
            cpu_set_t cpuset;
            CPU_ZERO(&cpuset);
            CPU_SET(0, &cpuset);
            pthread_attr_setaffinity_np(&attrs[i], sizeof(cpu_set_t), &cpuset);
        }
#endif

        int rc = pthread_create(&threads[i], &attrs[i], thread_func, NULL);
        if (rc != 0) {
            fprintf(stderr, "pthread_create error: %s\n", strerror(rc));
            return EXIT_FAILURE;
        }

        //Jeśli wątek jest joinable, zapamiętaj go do join później 
        if (i != 1) { // drugi wątek jest detached
            pthread_join(threads[i], NULL);
        }
        pthread_attr_destroy(&attrs[i]);
    }

    printf("\nWszystkie wątki zakończone.\n");
    return EXIT_SUCCESS;
}
