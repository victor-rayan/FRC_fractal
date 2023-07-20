#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    int server_socket, client_socket, read_size;
    struct sockaddr_in server_addr, client_addr;
    socklen_t client_addr_len = sizeof(client_addr);
    char buffer[MAX_BUFFER_SIZE];

    // Crie o socket TCP
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket == -1) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Configure a estrutura de endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(PORT);

    // Associe o socket ao endereço local
    if (bind(server_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro ao associar o socket ao endereço local");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Aguarde a conexão de um cliente
    if (listen(server_socket, 1) == -1) {
        perror("Erro ao aguardar conexão");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Aceite a conexão do cliente
    client_socket = accept(server_socket, (struct sockaddr*)&client_addr, &client_addr_len);
    if (client_socket == -1) {
        perror("Erro ao aceitar a conexão");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    // Receba o arquivo BMP do cliente e salve-o no disco
    FILE* file = fopen("received_fractal.bmp", "wb");
    if (!file) {
        perror("Erro ao criar o arquivo");
        close(client_socket);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    while ((read_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, read_size, file);
    }

    fclose(file);

    // Feche os sockets
    close(client_socket);
    close(server_socket);

    return 0;
}
