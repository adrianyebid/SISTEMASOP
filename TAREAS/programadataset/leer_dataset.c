#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 4096       // Aumentamos el tamaño por la cantidad de columnas y datos largos
#define REGISTROS_INICIALES 1000   // Tamaño inicial del array de registros

// Estructura que representa una fila del dataset de Discogs
typedef struct {
    char artist_anv[100];
    int artist_id;
    char artist_join[50];
    char artist_name[100];
    char artist_role[50];
    char artist_tracks[200];
    char company_catno[50];
    int company_entity_type;
    char company_entity_type_name[100];
    int company_id;
} Registro;

int main() {
    FILE *file = fopen("discogs_dataset.csv", "r");
    if (file == NULL) {
        perror("Error al abrir el archivo");
        return 1;
    }

    Registro *registros = malloc(REGISTROS_INICIALES * sizeof(Registro));
    if (registros == NULL) {
        perror("Error al asignar memoria");
        fclose(file);
        return 1;
    }

    int capacidad = REGISTROS_INICIALES;
    int contador = 0;
    char line[MAX_LINE_LENGTH];

    // Leer y descartar la línea de cabecera
    fgets(line, sizeof(line), file);

    printf("Procesando archivo...\n");

    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = '\0';  // Eliminar salto de línea

        // Separar por comas
        char *token = strtok(line, ",");
        int col = 0;

        // Variables temporales
        Registro temp = {0}; // Inicializa todos los campos a 0 o cadenas vacías

        while (token && col < 10) {
            switch (col) {
                case 0: strncpy(temp.artist_anv, token, sizeof(temp.artist_anv)-1); break;
                case 1: temp.artist_id = token[0] != '\0' ? atoi(token) : -1; break;
                case 2: strncpy(temp.artist_join, token, sizeof(temp.artist_join)-1); break;
                case 3: strncpy(temp.artist_name, token, sizeof(temp.artist_name)-1); break;
                case 4: strncpy(temp.artist_role, token, sizeof(temp.artist_role)-1); break;
                case 5: strncpy(temp.artist_tracks, token, sizeof(temp.artist_tracks)-1); break;
                case 6: strncpy(temp.company_catno, token, sizeof(temp.company_catno)-1); break;
                case 7: temp.company_entity_type = token[0] != '\0' ? atoi(token) : -1; break;
                case 8: strncpy(temp.company_entity_type_name, token, sizeof(temp.company_entity_type_name)-1); break;
                case 9: temp.company_id = token[0] != '\0' ? atoi(token) : -1; break;
            }
            token = strtok(NULL, ",");
            col++;
        }

        if (col >= 10) {
            if (contador >= capacidad) {
                capacidad *= 2;
                Registro *tmp = realloc(registros, capacidad * sizeof(Registro));
                if (tmp == NULL) {
                    perror("Error al redimensionar memoria");
                    free(registros);
                    fclose(file);
                    return 1;
                }
                registros = tmp;
            }

            registros[contador++] = temp;
        } else {
            printf("Línea incompleta o mal formateada: %s\n", line);
        }
    }

    fclose(file);

    // Mostrar los primeros 10 registros para verificar
    printf("\nPrimeros 10 registros:\n");
    for (int i = 0; i < 10 && i < contador; i++) {
        printf("Artist Name: %s | ID: %d | Role: %s | Company: %s (%s)\n",
               registros[i].artist_name,
               registros[i].artist_id,
               registros[i].artist_role,
               registros[i].company_catno,
               registros[i].company_entity_type_name);
    }

    printf("\nTotal de registros cargados: %d\n", contador);

    free(registros); // Liberar memoria
    return 0;
}
