#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    int client_socket;
    struct sockaddr_in server_addr;
    char buffer[MAX_BUFFER_SIZE];

    // Crie o socket TCP
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket == -1) {
        perror("Erro ao criar o socket");
        exit(EXIT_FAILURE);
    }

    // Configure a estrutura de endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Endereço IP do servidor (neste exemplo, local)
    server_addr.sin_port = htons(PORT);

    // Conecte-se ao servidor
    if (connect(client_socket, (struct sockaddr*)&server_addr, sizeof(server_addr)) == -1) {
        perror("Erro ao conectar-se ao servidor");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Abra o arquivo BMP a ser enviado para o servidor
    FILE* file = fopen("fractal.bmp", "rb");
    if (!file) {
        perror("Erro ao abrir o arquivo");
        close(client_socket);
        exit(EXIT_FAILURE);
    }

    // Envie o arquivo BMP para o servidor
    size_t read_size;
    while ((read_size = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        if (send(client_socket, buffer, read_size, 0) == -1) {
            perror("Erro ao enviar o arquivo");
            fclose(file);
            close(client_socket);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);

    // Feche o socket
    close(client_socket);

    return 0;
}
