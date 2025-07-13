// leibniz_pi_threads.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define NUM_TERMS 1000000000UL // 1e9

double pi_total = 0.0;
pthread_mutex_t mutex;

void* calcular_pi(void* arg) {
    long id = (long)arg;
    double suma_local = 0.0;

    for (unsigned long i = id; i < NUM_TERMS; i += NUM_THREADS) {
        double term = (i % 2 == 0 ? 1.0 : -1.0) / (2.0 * i + 1.0);
        suma_local += term;
    }

    pthread_mutex_lock(&mutex);
    pi_total += suma_local;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t hilos[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&hilos[i], NULL, calcular_pi, (void*)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    pi_total *= 4.0;
    printf("Aproximación de PI usando Leibniz con %lu términos y %d hilos: %.15f\n",
           NUM_TERMS, NUM_THREADS, pi_total);

    return 0;
}
