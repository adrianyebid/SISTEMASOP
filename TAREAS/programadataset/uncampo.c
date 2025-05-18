#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 2048
#define MAX_MOSTRAR 100  // Cambiar a -1 si querés ver todo

int main() {
    FILE *file = fopen("discogs_dataset.csv", "r"); 
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int linea_actual = 0;

    // Leer y descartar cabecera
    fgets(line, sizeof(line), file);

    printf("Mostrando solo el campo 'artist_name':\n\n");

    while (fgets(line, sizeof(line), file) != NULL && (MAX_MOSTRAR < 0 || linea_actual < MAX_MOSTRAR)) {
        line[strcspn(line, "\n")] = '\0';  // Quitar salto de línea

        char *campos[10] = {NULL};
        char *token = strtok(line, ",");
        int i = 0;

        while (token != NULL && i < 10) {
            campos[i++] = token;
            token = strtok(NULL, ",");
        }

        if (i > 3) {
            printf("%d: %s\n", linea_actual + 1, campos[3]);  // campo 4 = artist_name
        }

        linea_actual++;
    }

    fclose(file);
    return 0;
}
