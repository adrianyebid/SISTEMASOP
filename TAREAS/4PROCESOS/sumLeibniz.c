#include <stdio.h>
#include <time.h>

int main() {
    long long int iteraciones = 4000000000LL; // 4e9
    double pi = 0.0;
    int signo = 1;

    clock_t inicio = clock();

    for (long long int i = 0; i < iteraciones; i++) {
        pi += signo * (1.0 / (2 * i + 1));
        signo = -signo; // alternar signo
    }

    pi *= 4.0;

    clock_t fin = clock();
    double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

    printf("Valor aproximado de PI: %.15f\n", pi);
    printf("Execution time: %.2f segundos\n", tiempo);

    return 0;
}
