#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_LINE_LENGTH 2048
#define MAX_FIELD_LENGTH 256

typedef struct {
    char artist_anv[MAX_FIELD_LENGTH];
    char artist_id[MAX_FIELD_LENGTH];
    char artist_join[MAX_FIELD_LENGTH];
    char artist_name[MAX_FIELD_LENGTH];
    char artist_role[MAX_FIELD_LENGTH];
    char artist_tracks[MAX_FIELD_LENGTH];
    char company_catno[MAX_FIELD_LENGTH];
    char company_entity_type[MAX_FIELD_LENGTH];
    char company_entity_type_name[MAX_FIELD_LENGTH];
    char company_id[MAX_FIELD_LENGTH];
} Record;

void trim_newline(char *str) {
    str[strcspn(str, "\r\n")] = 0;
}

void parse_csv_line(char *line, Record *record) {
    // Separar los campos por coma
    char *token = strtok(line, ",");
    int field_index = 0;

    while (token != NULL && field_index < 10) {
        trim_newline(token);  // Eliminar saltos de línea
        if (strcmp(token, "NULL") == 0 || strlen(token) == 0)
            token = "";

        switch (field_index) {
            case 0: strncpy(record->artist_anv, token, MAX_FIELD_LENGTH); break;
            case 1: strncpy(record->artist_id, token, MAX_FIELD_LENGTH); break;
            case 2: strncpy(record->artist_join, token, MAX_FIELD_LENGTH); break;
            case 3: strncpy(record->artist_name, token, MAX_FIELD_LENGTH); break;
            case 4: strncpy(record->artist_role, token, MAX_FIELD_LENGTH); break;
            case 5: strncpy(record->artist_tracks, token, MAX_FIELD_LENGTH); break;
            case 6: strncpy(record->company_catno, token, MAX_FIELD_LENGTH); break;
            case 7: strncpy(record->company_entity_type, token, MAX_FIELD_LENGTH); break;
            case 8: strncpy(record->company_entity_type_name, token, MAX_FIELD_LENGTH); break;
            case 9: strncpy(record->company_id, token, MAX_FIELD_LENGTH); break;
        }

        token = strtok(NULL, ",");
        field_index++;
    }
}

void print_record(const Record *record, int index) {
    printf("\nRegistro #%d\n", index);
    printf("  artist_anv: %s\n", record->artist_anv);
    printf("  artist_id: %s\n", record->artist_id);
    printf("  artist_join: %s\n", record->artist_join);
    printf("  artist_name: %s\n", record->artist_name);
    printf("  artist_role: %s\n", record->artist_role);
    printf("  artist_tracks: %s\n", record->artist_tracks);
    printf("  company_catno: %s\n", record->company_catno);
    printf("  company_entity_type: %s\n", record->company_entity_type);
    printf("  company_entity_type_name: %s\n", record->company_entity_type_name);
    printf("  company_id: %s\n", record->company_id);
}

int main() {
    FILE *file = fopen("discogs_dataset.csv", "r");
    if (!file) {
        perror("No se pudo abrir el archivo");
        return 1;
    }

    char line[MAX_LINE_LENGTH];
    Record record;
    int count = 0;

    // Leer cabecera
    fgets(line, sizeof(line), file);

    while (fgets(line, sizeof(line), file) && count < 5) {
        parse_csv_line(line, &record);
        print_record(&record, count + 1);
        count++;
    }

    fclose(file);
    return 0;
}
