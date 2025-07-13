// leibniz_pi_ranges.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define NUM_TERMS 4000000000UL // 4e9

double pi_total = 0.0;
pthread_mutex_t mutex;

struct Rango {
    unsigned long inicio;
    unsigned long fin;
};

void* calcular_rango_pi(void* arg) {
    struct Rango* rango = (struct Rango*)arg;
    double suma_local = 0.0;

    for (unsigned long i = rango->inicio; i < rango->fin; i++) {
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
    struct Rango rangos[NUM_THREADS];

    pthread_mutex_init(&mutex, NULL);

    unsigned long paso = NUM_TERMS / NUM_THREADS;

    for (long i = 0; i < NUM_THREADS; i++) {
        rangos[i].inicio = i * paso;
        rangos[i].fin = (i + 1) * paso;
        pthread_create(&hilos[i], NULL, calcular_rango_pi, &rangos[i]);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    pi_total *= 4.0;
    printf("Aproximación de PI con %lu términos y %d hilos (división por rango):\n%.15f\n",
           NUM_TERMS, NUM_THREADS, pi_total);

    return 0;
}
