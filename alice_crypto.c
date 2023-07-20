#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/des.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    // Código de inicialização do servidor TCP aqui (similar ao exemplo anterior)

    // Defina a chave de criptografia DES
    unsigned char des_key[DES_KEY_SZ] = "MySecretK3y";

    // Inicialize a estrutura DES_key_schedule com a chave
    DES_key_schedule des_ks;
    DES_set_key_checked((const_DES_cblock*)des_key, &des_ks);

    // Receba o arquivo BMP do cliente e descriptografe-o no disco
    FILE* file = fopen("received_fractal.bmp", "wb");
    if (!file) {
        perror("Erro ao criar o arquivo");
        close(client_socket);
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    while ((read_size = recv(client_socket, buffer, sizeof(buffer), 0)) > 0) {
        // Descriptografe o buffer recebido usando DES
        DES_ecb_encrypt((const_DES_cblock*)buffer, (DES_cblock*)buffer, &des_ks, DES_DECRYPT);
        fwrite(buffer, 1, read_size, file);
    }

    fclose(file);

    // continue

    return 0;
}
