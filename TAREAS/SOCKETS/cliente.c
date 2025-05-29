#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>

#define SERVER_IP "127.0.0.1"  // Localhost
#define PORT 3550
#define BUFFER_SIZE 1024

int client_fd;

void signal_handler(int signum) {
    printf("\nCerrando cliente...\n");
    close(client_fd);
    exit(signum);
}

int main() {
    signal(SIGINT, signal_handler);
    
    struct sockaddr_in server_addr;
    char buffer[BUFFER_SIZE];
    int r;

    // Crear socket
    client_fd = socket(AF_INET, SOCK_STREAM, 0);
    if(client_fd == -1) {
        perror("Error al crear socket");
        return -1;
    }

    // Configurar dirección del servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if(inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
        perror("Dirección inválida");
        close(client_fd);
        return -1;
    }

    // Conectar al servidor
    if(connect(client_fd, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        perror("Conexión fallida");
        close(client_fd);
        return -1;
    }

    printf("Conectado al servidor. Escribe mensajes (o 'exit' para salir):\n");

    while(1) {
        printf("> ");
        fgets(buffer, BUFFER_SIZE, stdin);
        
        // Eliminar salto de línea
        buffer[strcspn(buffer, "\n")] = '\0';
        
        if(strcmp(buffer, "exit") == 0) break;

        // Enviar mensaje
        r = send(client_fd, buffer, strlen(buffer), 0);
        if(r == -1) {
            perror("Error al enviar");
            break;
        }

        // Recibir respuesta
        r = recv(client_fd, buffer, BUFFER_SIZE-1, 0);
        if(r <= 0) {
            if(r == 0) printf("Servidor desconectado\n");
            else perror("Error al recibir");
            break;
        }

        buffer[r] = '\0';
        printf("Respuesta: %s\n", buffer);
    }

    close(client_fd);
    return 0;
}