#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

#define PROCESOS 4
#define ITERACIONES 4000000000LL  // 4e9

int main() {
    int pipes[PROCESOS][2];
    pid_t pids[PROCESOS];
    long long int iteraciones = ITERACIONES;
    double resultado_total = 0.0;
    struct timeval start, end;
    gettimeofday(&start, NULL);

    clock_t inicio = clock();

    for (int i = 0; i < PROCESOS; i++) {
        if (pipe(pipes[i]) < 0) {
            perror("error al crear la tuberia");
            exit(1);
        }

        pids[i] = fork();

        if (pids[i] < 0) {
            perror("error al crear un proceso hijo");
            exit(1);
        }

        if (pids[i] == 0) {
            // Código del hijo
            close(pipes[i][0]); // Cerramos lectura

            long long int start = i * (iteraciones / PROCESOS);
            long long int end = (i + 1) * (iteraciones / PROCESOS);
            double suma = 0.0;

            for (long long int j = start; j < end; j++) {
                double term = (j % 2 == 0 ? 1.0 : -1.0) / (2 * j + 1);
                suma += term;
            }

            // Enviar resultado al padre
            write(pipes[i][1], &suma, sizeof(double));
            close(pipes[i][1]); // Cerramos escritura
            exit(0); // Hijo termina
        }

        // El padre no usa la escritura de la tubería
        close(pipes[i][1]);
    }

    // Esperar a los hijos y leer resultados
    for (int i = 0; i < PROCESOS; i++) {
        //waitpid(pids[i], NULL, 0);

        double parcial = 0.0;
        read(pipes[i][0], &parcial, sizeof(double));
        resultado_total += parcial;

        close(pipes[i][0]);
    }

    resultado_total *= 4.0;

    clock_t fin = clock();
    double tiempo = (double)(fin - inicio) / CLOCKS_PER_SEC;

    printf("Valor aproximado de PI: %.15f\n", resultado_total);
    printf("Tiempo de ejecución (pipe): %.2f segundos\n", tiempo);

    return 0;
}
