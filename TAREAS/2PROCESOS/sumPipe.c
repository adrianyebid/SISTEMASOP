#include <unistd.h>     // Para pipe(), fork(), read(), write(), close()
#include <stdio.h>      // Para printf(), perror()
#include <stdlib.h>     // Para exit()
#include <sys/types.h>  // Para pid_t
#include <sys/wait.h>   // Para el wait

int main() {
    int r,w;
    int pipefd[2];       // pipefd[0] para leer, pipefd[1] para escribir
    int pipefd2[2];       // pipefd[0] para leer, pipefd[1] para escribir
    pid_t pid;
    int sumTotal = 0;

    // Crear el pipe 1 
    r = pipe(pipefd);
    
    // Crear el pipe 2 
    w = pipe(pipefd2);
    
    if (r == -1 || w == -1) {
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
        
        //Proceso hijo - leer del pipe 1 y escribir resultado pipe 2

        close(pipefd[1]); // Cerrar el extremo de escritura pipe 1

        close(pipefd2[0]); //cerramos extremo de lectura pipe 2    
        
        //leemos el resultado del padre del pipe 1
        r = read(pipefd[0], &sumTotal, sizeof(int));
        
        if (r == -1) {
            perror("Error al leer del pipe");
            exit(EXIT_FAILURE);
        }

        printf("Hijo recibió suma parcial: %d\n", sumTotal);

        for (int i = 500; i <= 999; i++) sumTotal += i;

        //escribimos la suma total en el pipe 2
        w = write(pipefd2[1], &sumTotal, sizeof(int));
        
        close(pipefd[0]); // Cerrar el extremo de lectura pipe 1

        close(pipefd2[1]); // Cerrar el extremo de escritura pipe 2

        exit(0);
    } else {
        
        // Proceso padre - escribir en el pipe 1 y leer del pipe 2

        close(pipefd[0]); // Cerrar el extremo de lectura pipe 1

        close(pipefd2[1]); // Cerrar el extremo de escritura pipe 2

        for (int i = 0; i <= 499; i++) sumTotal += i;
        
        // escribimos la primera suma en el pipe 1
        r = write(pipefd[1], &sumTotal, sizeof(int));
        
        if (r == -1) {
            perror("Error al escribir en el pipe");
            exit(EXIT_FAILURE);
        }

    
        printf("Padre envió suma parcial: %d\n", sumTotal);

        // Esperamos al chamaco 
        wait(NULL);

        // leemos la suma total enviado por el hijo en el pipe 2
        w = read(pipefd2[0], &sumTotal, sizeof(int));

        printf("La suma total desde el padre es: %d\n", sumTotal);

        close(pipefd[1]); // Cerrar el extremo de escritura pipe 1
        close(pipefd2[0]); // Cerrar el extremo de lectura  pipe 2

        exit(0);
    }

    return 0;
}