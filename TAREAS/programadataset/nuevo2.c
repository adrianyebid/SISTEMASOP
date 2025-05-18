#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 4096
#define FIELDS_COUNT 11

void print_field(const char* field_name, const char* value) {
    printf("%-25s: ", field_name);
    if (value == NULL || strcmp(value, "NULL") == 0) {
        printf("[NULL]\n");
    } else {
        printf("%s\n", value);
    }
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Uso: %s <archivo.csv>\n", argv[0]);
        return 1;
    }

    FILE *file = fopen(argv[1], "r");
    if (!file) {
        perror("Error al abrir el archivo");
        return 1;
    }

    printf("Archivo abierto correctamente. Leyendo...\n");

    char line[MAX_LINE_LENGTH];
    int rows_to_show = 5;
    int row_count = 0;

    while (fgets(line, sizeof(line), file) && row_count < rows_to_show) {
        printf("\nProcesando línea %d:\n", row_count + 1);
        printf("Contenido crudo: %s\n", line);

        // Eliminar salto de línea
        line[strcspn(line, "\n")] = '\0';

        char *fields[FIELDS_COUNT] = {NULL};
        char *token = strtok(line, "\t"); // Primero prueba con tabulaciones
        
        if (token == NULL) {
            token = strtok(line, ",");    // Si no funciona, prueba con comas
        }

        int field_count = 0;
        while (token != NULL && field_count < FIELDS_COUNT) {
            fields[field_count] = token;
            printf("Campo %d: %s\n", field_count, token);
            token = strtok(NULL, "\t");
            if (token == NULL && field_count == 0) {
                token = strtok(NULL, ",");
            }
            field_count++;
        }

        if (field_count > 0) {
            printf("\n=== Fila %d ===\n", row_count + 1);
            print_field("artist_anv", fields[0]);
            print_field("artist_id", fields[1]);
            print_field("artist_join", fields[2]);
            print_field("artist_name", fields[3]);
            print_field("artist_role", fields[4]);
            print_field("artist_tracks", fields[5]);
            print_field("company_catno", fields[6]);
            print_field("company_entity_type", fields[7]);
            print_field("company_entity_type_name", fields[8]);
            print_field("company_id", fields[9]);
        } else {
            printf("Error: No se encontraron campos en esta línea.\n");
        }

        row_count++;
    }

    if (row_count == 0) {
        printf("No se encontraron datos o el archivo está vacío.\n");
    }

    fclose(file);
    return 0;
}