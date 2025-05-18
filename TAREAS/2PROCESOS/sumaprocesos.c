#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>

#define ARCHIVO_TEMP "suma_temp.bin"  // Cambiamos la extensión a .bin por claridad

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

        // Escribe resultado en archivo con fwrite (modo binario)
        FILE *f = fopen(ARCHIVO_TEMP, "wb");  // 'b' para modo binario
        if (f == NULL) {
            perror("Error al crear archivo");
            exit(1);
        }
        fwrite(&suma, sizeof(int), 1, f);  // Escribe el entero en binario, (const void *ptr, size_t size, size_t nmemb (# elementos a escribir), FILE *stream (puntero donde escribiremos))
        fclose(f);
        
        printf("Hijo: Mi suma es %d\n", suma);
        exit(0);
    } 
    else {
        // Proceso padre: suma 0-499
        int suma_padre = 0;
        for (int i = 0; i <= 499; i++) suma_padre += i;
        printf("Padre: Mi suma es %d\n", suma_padre);

        // Espera al hijo
        wait(NULL);

        // Lee resultado del hijo con fread
        FILE *f = fopen(ARCHIVO_TEMP, "rb");
        if (f == NULL) {
            perror("Error al leer archivo");
            return 1;
        }
        int suma_hijo;
        fread(&suma_hijo, sizeof(int), 1, f);  // Lee el entero binario
        fclose(f);

        // Elimina el archivo temporal
        remove(ARCHIVO_TEMP);

        // Suma total
        printf("Suma total: %d + %d = %d\n", 
               suma_padre, suma_hijo, suma_padre + suma_hijo);
    }

    return 0;
}