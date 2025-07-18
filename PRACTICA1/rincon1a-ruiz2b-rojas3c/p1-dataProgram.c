// p1-dataProgram.c
// Práctica 1: Búsqueda por VIN (tabla hash) y por marca (make) usando procesos no emparentados y FIFO

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <ctype.h>

#define MAX_LINE 512
#define MAX_VIN 32
#define MAX_MAKE 64
#define HASH_SIZE 100003  // Tamaño de la tabla hash (número primo para mejor dispersión)

// Estructura para los registros indexados por VIN
typedef struct Registro {
    char vin[MAX_VIN];       // VIN del vehículo
    long offset;             // Posición del registro en el archivo
    struct Registro* next;   // Puntero para manejar colisiones (lista enlazada)
} Registro;

Registro* hash_table[HASH_SIZE];  // Tabla hash para buscar por VIN

// Algoritmo DJB2 para obtener un índice hash a partir del VIN
unsigned long hash_vin(const char* vin) {
    unsigned long hash = 5381;
    int c;
    while ((c = *vin++))
        hash = ((hash << 5) + hash) + c;  // hash * 33 + c
    return hash % HASH_SIZE;
}

// Limpia el VIN eliminando comillas, saltos de línea y convierte a mayúsculas
void limpiar_vin(char* vin) {
    if (vin[0] == '"') {
        memmove(vin, vin + 1, strlen(vin));
        vin[strcspn(vin, "\"")] = '\0';
    }
    vin[strcspn(vin, "\n\r")] = '\0';
    for (int i = 0; vin[i]; i++)
        vin[i] = toupper(vin[i]);
}

// Inserta un VIN y su offset en la tabla hash
void insertar_hash(const char* vin, long offset) {
    char vin_limpio[MAX_VIN];
    strncpy(vin_limpio, vin, MAX_VIN - 1);
    vin_limpio[MAX_VIN - 1] = '\0';
    limpiar_vin(vin_limpio);

    unsigned long index = hash_vin(vin_limpio);

    // Crea nuevo nodo y lo inserta al inicio de la lista en hash_table[index]
    Registro* nuevo = malloc(sizeof(Registro));
    strncpy(nuevo->vin, vin_limpio, MAX_VIN - 1);
    nuevo->vin[MAX_VIN - 1] = '\0';
    nuevo->offset = offset;
    nuevo->next = hash_table[index];  // Encadenamiento (colisiones)
    hash_table[index] = nuevo;
}

// Busca un VIN en la tabla hash y devuelve su offset
long buscar_por_vin(const char* vin) {
    char vin_limpio[MAX_VIN];
    strncpy(vin_limpio, vin, MAX_VIN - 1);
    vin_limpio[MAX_VIN - 1] = '\0';
    limpiar_vin(vin_limpio);

    unsigned long index = hash_vin(vin_limpio);
    Registro* actual = hash_table[index];
    while (actual) {
        if (strcmp(actual->vin, vin_limpio) == 0)
            return actual->offset;
        actual = actual->next;
    }
    return -1;  // No encontrado
}

// Busca todos los registros con cierta marca (make) y los imprime
void buscar_por_make(const char* archivo_csv, const char* make, FILE* out) {
    FILE* f = fopen(archivo_csv, "r");
    if (!f) { perror("fopen"); exit(1); }

    char linea[MAX_LINE];
    fgets(linea, sizeof(linea), f);  // Leer encabezado

    int encontrados = 0;
    while (fgets(linea, sizeof(linea), f)) {
        char* linea_copia = strdup(linea);
        char* token = strtok(linea_copia, ",");
        int i = 0;
        char campo_make[MAX_MAKE] = "";

        // Extrae la marca (make) del campo i = 1
        while (token) {
            if (i == 1) {
                strncpy(campo_make, token, MAX_MAKE - 1);
                campo_make[MAX_MAKE - 1] = '\0';
                break;
            }
            token = strtok(NULL, ",");
            i++;
        }

        if (strcmp(campo_make, make) == 0) {
            fprintf(out, "%s", linea);  // Coincidencia encontrada
            encontrados++;
        }
        free(linea_copia);
    }

    if (encontrados == 0) fprintf(out, "NA\n");
    fclose(f);
}

// Construye el índice hash a partir del archivo CSV
void construir_indice(const char* archivo_csv) {
    FILE* f = fopen(archivo_csv, "r");
    if (!f) { perror("fopen"); exit(1); }

    char linea[MAX_LINE];
    long offset;

    fgets(linea, sizeof(linea), f);  // Encabezado

    // Itera línea por línea guardando el VIN y su offset
    while ((offset = ftell(f)), fgets(linea, sizeof(linea), f)) {
        char* linea_copia = strdup(linea);
        char* token = strtok(linea_copia, ",");
        int i = 0;
        char vin[MAX_VIN] = "";
        while (token) {
            if (i == 5) {  // VIN está en la columna 5
                strncpy(vin, token, MAX_VIN - 1);
                vin[MAX_VIN - 1] = '\0';
                break;
            }
            token = strtok(NULL, ",");
            i++;
        }
        if (strlen(vin) > 0)
            insertar_hash(vin, offset);
        free(linea_copia);
    }
    fclose(f);
}

int main() {
    const char* archivo_csv = "car_prices_extend_real.csv";
    const char* fifo_peticion = "/tmp/fifo_peticion";
    const char* fifo_respuesta = "/tmp/fifo_respuesta";

    mkfifo(fifo_peticion, 0666);   // Crea FIFO para recibir consultas
    mkfifo(fifo_respuesta, 0666);  // Crea FIFO para enviar respuestas

    printf("Construyendo índice por VIN...\n");
    construir_indice(archivo_csv);
    printf("Índice listo. Esperando consultas...\n");

    while (1) {
        char buffer[128];
        int fd_pet = open(fifo_peticion, O_RDONLY);
        ssize_t leido = read(fd_pet, buffer, sizeof(buffer));
        close(fd_pet);
        if (leido <= 0) continue;

        buffer[strcspn(buffer, "\n")] = 0;  // Quita salto de línea

        // Divide el mensaje recibido: "vin <dato>" o "make <dato>"
        char tipo[8] = "", dato[64] = "";
        char* token = strtok(buffer, " ");
        char* valor = strtok(NULL, "\n");
        if (token) strncpy(tipo, token, sizeof(tipo) - 1);
        if (valor) strncpy(dato, valor, sizeof(dato) - 1);

        FILE* out = fopen(fifo_respuesta, "w");

        // Procesa la consulta por VIN
        if (strcmp(tipo, "vin") == 0) {
            long offset = buscar_por_vin(dato);
            if (offset == -1) {
                fprintf(out, "NA\n");
            } else {
                FILE* f = fopen(archivo_csv, "r");
                fseek(f, offset, SEEK_SET);  // Salta al offset del registro
                char resultado[MAX_LINE];
                if (fgets(resultado, sizeof(resultado), f))
                    fprintf(out, "%s", resultado);
                else
                    fprintf(out, "NA\n");
                fclose(f);
            }
        } else if (strcmp(tipo, "make") == 0) {
            buscar_por_make(archivo_csv, dato, out);
        }

        fclose(out);
    }

    return 0;
}
