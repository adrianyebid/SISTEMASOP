#include <sys/socket.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define PORT 3550
#define BACKLOG 5
#define BUFFER_SIZE 1024

int server_fd;

void signal_handler(int signum) {
    printf("\nCerrando servidor...\n");
    close(server_fd);
    exit(signum);
}

int main() {
    signal(SIGINT, signal_handler);
    
    struct sockaddr_in server_addr, client_addr;
    int client_fd;
    socklen_t client_len = sizeof(client_addr);
    char buffer[BUFFER_SIZE];
    int opt = 1;
    int r;

    // Crear socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(server_fd == -1) {
        perror("Error al crear socket");
        return -1;
    }

    // Configurar opción para reutilizar puerto
    if(setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt))) {
        perror("setsockopt");
        close(server_fd);
        return -1;
    }

    // Configurar dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;
    memset(server_addr.sin_zero, '\0', sizeof(server_addr.sin_zero));

    // Vincular socket
    if(bind(server_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Error en bind");
        close(server_fd);
        return -1;
    }

    // Escuchar conexiones
    if(listen(server_fd, BACKLOG) == -1) {
        perror("Error en listen");
        close(server_fd);
        return -1;
    }

    printf("Servidor iniciado en localhost:%d\n", PORT);
    printf("Esperando conexiones...\n");

    while(1) {
        // Aceptar nueva conexión
        client_fd = accept(server_fd, (struct sockaddr*)&client_addr, &client_len);
        if(client_fd < 0) {
            perror("Error en accept");
            continue;
        }

        printf("\nCliente conectado!\n");

        // Comunicación con el cliente
        while(1) {
            // Recibir mensaje
            r = recv(client_fd, buffer, BUFFER_SIZE, 0);
            if(r <= 0) {
                if(r == 0) printf("Cliente desconectado\n");
                else perror("Error en recv");
                break;
            }

            buffer[r] = '\0';
            printf("Mensaje recibido: %s\n", buffer);

            // Enviar respuesta
            char response[BUFFER_SIZE];
            snprintf(response, BUFFER_SIZE, "Servidor: Recibido - '%s'", buffer);
            r = send(client_fd, response, strlen(response), 0);
            if(r == -1) {
                perror("Error en send");
                break;
            }
        }

        close(client_fd);
        printf("Esperando nueva conexión...\n");
    }

    close(server_fd);
    return 0;
}