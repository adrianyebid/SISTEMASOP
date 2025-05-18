#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int main() {
    
    printf("Proceso original (futuro padre) PID: %d\n", getpid());

    pid_t pid = fork(); // Crear primer proceso hijo

    if (pid < 0) {
        // Error al crear el proceso
        fprintf(stderr, "Error al crear el proceso\n");
        return 1;
    } else if (pid == 0) {
        // Código del hijo
        printf("Hola soy tu hijo\n");
        printf("HIJO: Mi PID es %d\n", getpid());
        printf("HIJO: Mi padre tiene PID %d\n", getppid());
    } else {
        // Código del padre
        printf("Hola soy tu padre\n");
        wait(NULL); // Esperar a que el hijo termine
          printf("PADRE: Mi PID es %d\n", getpid());
        printf("PADRE: Mi hijo tiene PID %d\n", pid);
    }

    return 0;
}