#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 2048
#define MAX_MOSTRAR 10  // Cuántas filas mostrar

int main() {
    FILE *file = fopen("discogs_dataset.csv", "r");  // Cambiá el nombre si es otro
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    int linea_actual = 0;

    // Leer y descartar encabezado
    fgets(line, sizeof(line), file);

    printf("Visualizando las primeras %d líneas del CSV:\n\n", MAX_MOSTRAR);

    while (fgets(line, sizeof(line), file) != NULL && linea_actual < MAX_MOSTRAR) {
        line[strcspn(line, "\n")] = '\0';  // Eliminar salto de línea

        // Separar los campos por coma
        char *campos[10] = {NULL};
        char *token = strtok(line, ",");
        int i = 0;

        while (token != NULL && i < 10) {
            campos[i++] = token;
            token = strtok(NULL, ",");
        }

        if (i >= 1) {
            printf("Registro #%d:\n", linea_actual + 1);
            if (i > 0) printf("  artist_anv: %s\n", campos[0] ? campos[0] : "(null)");
            if (i > 1) printf("  artist_id: %s\n", campos[1] ? campos[1] : "(null)");
            if (i > 2) printf("  artist_join: %s\n", campos[2] ? campos[2] : "(null)");
            if (i > 3) printf("  artist_name: %s\n", campos[3] ? campos[3] : "(null)");
            if (i > 4) printf("  artist_role: %s\n", campos[4] ? campos[4] : "(null)");
            if (i > 5) printf("  artist_tracks: %s\n", campos[5] ? campos[5] : "(null)");
            if (i > 6) printf("  company_catno: %s\n", campos[6] ? campos[6] : "(null)");
            if (i > 7) printf("  company_entity_type: %s\n", campos[7] ? campos[7] : "(null)");
            if (i > 8) printf("  company_entity_type_name: %s\n", campos[8] ? campos[8] : "(null)");
            if (i > 9) printf("  company_id: %s\n", campos[9] ? campos[9] : "(null)");
            printf("----------------------------------------------------\n");
            linea_actual++;
        }
        
    }

    fclose(file);
    return 0;
}
