#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/des.h>

#define PORT 12345
#define MAX_BUFFER_SIZE 1024

int main() {
    // Código de inicialização do cliente TCP aqui (similar ao exemplo anterior)

    // Defina a chave de criptografia DES
    unsigned char des_key[DES_KEY_SZ] = "MySecretK3y";

    // Inicialize a estrutura DES_key_schedule com a chave
    DES_key_schedule des_ks;
    DES_set_key_checked((const_DES_cblock*)des_key, &des_ks);

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
        // Criptografe o buffer antes de enviar usando DES
        DES_ecb_encrypt((const_DES_cblock*)buffer, (DES_cblock*)buffer, &des_ks, DES_ENCRYPT);
        if (send(client_socket, buffer, read_size, 0) == -1) {
            perror("Erro ao enviar o arquivo");
            fclose(file);
            close(client_socket);
            exit(EXIT_FAILURE);
        }
    }

    fclose(file);

    // Resto do código do cliente aqui (similar ao exemplo anterior)

    return 0;
}
