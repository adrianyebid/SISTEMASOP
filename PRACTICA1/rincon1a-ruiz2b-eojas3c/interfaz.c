// interfaz.c
// Proceso de interfaz de usuario para enviar consultas por FIFO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FIFO_PETICION "/tmp/fifo_peticion"
#define FIFO_RESPUESTA "/tmp/fifo_respuesta"

void menu() {
    printf("\nBienvenido al sistema de búsqueda\n");
    printf("1. Buscar por VIN\n");
    printf("2. Buscar por Marca (Make)\n");
    printf("3. Salir\n");
    printf("Seleccione una opción: ");
}

int main() {
    char opcion[8];
    char entrada[64];

    while (1) {
        menu();
        fgets(opcion, sizeof(opcion), stdin);

        if (opcion[0] == '3') {
            printf("Saliendo...\n");
            break;
        }

        if (opcion[0] != '1' && opcion[0] != '2') {
            printf("Opción inválida.\n");
            continue;
        }

        if (opcion[0] == '1') {
            printf("Ingrese VIN: ");
            fgets(entrada, sizeof(entrada), stdin);
            entrada[strcspn(entrada, "\n")] = 0;

            int fd = open(FIFO_PETICION, O_WRONLY);
            dprintf(fd, "vin %s\n", entrada);
            close(fd);
        } else {
            printf("Ingrese marca (make): ");
            fgets(entrada, sizeof(entrada), stdin);
            entrada[strcspn(entrada, "\n")] = 0;

            int fd = open(FIFO_PETICION, O_WRONLY);
            dprintf(fd, "make %s\n", entrada);
            close(fd);
        }

        FILE* f = fopen(FIFO_RESPUESTA, "r");
        if (!f) { perror("fopen respuesta"); exit(1); }
        printf("\n🔎 Resultado:\n");
        char linea[512];
        while (fgets(linea, sizeof(linea), f)) {
            printf("%s", linea);
        }
        fclose(f);
    }

    return 0;
}
