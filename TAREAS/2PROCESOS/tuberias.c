#include <unistd.h>     // Para pipe(), fork(), read(), write(), close()
#include <stdio.h>      // Para printf(), perror()
#include <stdlib.h>     // Para exit()
#include <sys/types.h>  // Para pid_t

int main() {
    int r;
    int pipefd[2];       // pipefd[0] para leer, pipefd[1] para escribir
    pid_t pid;
    float dato;

    // Crear el pipe
    r = pipe(pipefd);
    if (r == -1) {
        perror("Error al crear el pipe");
        exit(EXIT_FAILURE);
    }

    // Crear el proceso hijo
    pid = fork();
    if (pid == -1) {
        perror("Error al hacer fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) {
        // Proceso hijo - leer del pipe
        close(pipefd[1]); // Cerrar el extremo de escritura
        r = read(pipefd[0], &dato, sizeof(float));
        if (r == -1) {
            perror("Error al leer del pipe");
            exit(EXIT_FAILURE);
        }
        close(pipefd[0]); // Cerrar el extremo de lectura

        printf("Dato recibido en el hijo: %f\n", dato);
        exit(0);
    } else {
        // Proceso padre - escribir en el pipe
        close(pipefd[0]); // Cerrar el extremo de lectura

        dato = 3.1415f; // Nota: usar punto decimal (coma es incorrecta)
        r = write(pipefd[1], &dato, sizeof(float));
        if (r == -1) {
            perror("Error al escribir en el pipe");
            exit(EXIT_FAILURE);
        }

        close(pipefd[1]); // Cerrar el extremo de escritura

        exit(0);
    }

    return 0;
}