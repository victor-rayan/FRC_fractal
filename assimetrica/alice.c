#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define PORT 8080
#define SERVER_IP "127.0.0.1" // Endereço IP de Bob (localhost)

int main() {
    // Inicializar OpenSSL
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    // Carregar chave pública de Bob
    FILE* key_file = fopen("chave.pub", "r");
    if (!key_file) {
        perror("Erro ao abrir a chave pública");
        return 1;
    }
    RSA* rsa_public_key = PEM_read_RSA_PUBKEY(key_file, NULL, NULL, NULL);
    fclose(key_file);
    if (!rsa_public_key) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // Criar o socket do cliente
    int client_socket;
    struct sockaddr_in server_address;

    if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Erro ao criar o socket");
        return 1;
    }

    server_address.sin_family = AF_INET;
    server_address.sin_port = htons(PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_address.sin_addr) <= 0) {
        perror("Endereço inválido");
        return 1;
    }

    if (connect(client_socket, (struct sockaddr*)&server_address, sizeof(server_address)) < 0) {
        perror("Falha ao conectar-se com Bob");
        return 1;
    }

    // Abrir o arquivo fractal para envio
    FILE* file_to_send = fopen("fractal.bmp", "rb");
    if (!file_to_send) {
        perror("Erro ao abrir o arquivo fractal.bmp");
        return 1;
    }

    // Criptografar e enviar o arquivo para Bob
    printf("Enviando o arquivo criptografado para Bob...\n");
    unsigned char buffer[256];
    int bytes_read, encrypted_len;

    while ((bytes_read = fread(buffer, 1, sizeof(buffer), file_to_send)) > 0) {
        unsigned char encrypted[256];
        encrypted_len = RSA_public_encrypt(bytes_read, buffer, encrypted, rsa_public_key, RSA_PKCS1_PADDING);
        send(client_socket, encrypted, encrypted_len, 0);
    }
    fclose(file_to_send);
    close(client_socket);

    printf("Arquivo enviado com sucesso!\n");

    // Limpar recursos do OpenSSL
    RSA_free(rsa_public_key);
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
