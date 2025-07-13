// euler_e_threads.c
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

#define NUM_THREADS 4
#define NUM_TERMS 20

double e_total = 1.0; // 1/0! = 1
pthread_mutex_t mutex;

unsigned long long factorial(int n) {
    unsigned long long f = 1;
    for (int i = 2; i <= n; i++)
        f *= i;
    return f;
}

void* calcular_e(void* arg) {
    long id = (long)arg;
    double suma_local = 0.0;

    for (int i = id + 1; i <= NUM_TERMS; i += NUM_THREADS) {
        suma_local += 1.0 / factorial(i);
    }

    pthread_mutex_lock(&mutex);
    e_total += suma_local;
    pthread_mutex_unlock(&mutex);

    return NULL;
}

int main() {
    pthread_t hilos[NUM_THREADS];
    pthread_mutex_init(&mutex, NULL);

    for (long i = 0; i < NUM_THREADS; i++) {
        pthread_create(&hilos[i], NULL, calcular_e, (void*)i);
    }

    for (int i = 0; i < NUM_THREADS; i++) {
        pthread_join(hilos[i], NULL);
    }

    pthread_mutex_destroy(&mutex);

    printf("Aproximación de e usando %d términos y %d hilos: %.15f\n",
           NUM_TERMS, NUM_THREADS, e_total);

    return 0;
}
