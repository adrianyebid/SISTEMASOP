#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define ARCHIVO_TEMP "suma_temp.txt"

int main() {
    pid_t pid = fork();

    if (pid < 0) {
        fprintf(stderr, "Error en fork()\n");
        return 1;
    } 
    else if (pid == 0) {
        // Proceso hijo: suma 500-999
        int suma = 0;
        for (int i = 500; i <= 999; i++) suma += i;

        // Escribe resultado en archivo
        FILE *f = fopen(ARCHIVO_TEMP, "w");
        if (f == NULL) {
            perror("Error al crear archivo");
            exit(1);
        }
        fprintf(f, "%d", suma);
        fclose(f);
        
        printf("Hijo: Mi suma es %d\n", suma);
        exit(0);  // Termina el hijo
    } 
    else {
        // Proceso padre: suma 0-499
        int suma_padre = 0;
        for (int i = 0; i <= 499; i++) suma_padre += i;
        printf("Padre: Mi suma es %d\n", suma_padre);

        // Espera al hijo
        wait(NULL);

        // Lee resultado del hijo
        FILE *f = fopen(ARCHIVO_TEMP, "r");
        if (f == NULL) {
            perror("Error al leer archivo");
            return 1;
        }
        int suma_hijo;
        fscanf(f, "%d", &suma_hijo);
        fclose(f);

        // Elimina el archivo temporal (opcional)
        remove(ARCHIVO_TEMP);

        // Suma total
        printf("Suma total: %d + %d = %d\n", 
               suma_padre, suma_hijo, suma_padre + suma_hijo);
    }

    return 0;
}