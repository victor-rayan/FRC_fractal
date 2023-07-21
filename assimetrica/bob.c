#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define PORT 8080

int main() {
    // Inicializar OpenSSL
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();

    // Carregar chave privada de Bob
    FILE* key_file = fopen("chave.priv", "r");
    if (!key_file) {
        perror("Erro ao abrir a chave privada");
        return 1;
    }
    RSA* rsa_private_key = PEM_read_RSAPrivateKey(key_file, NULL, NULL, NULL);
    fclose(key_file);
    if (!rsa_private_key) {
        ERR_print_errors_fp(stderr);
        return 1;
    }

    // Criar o socket do servidor
    int server_fd, new_socket;
    struct sockaddr_in address;
    int opt = 1;
    int addrlen = sizeof(address);

    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("Falha ao criar o socket");
        return 1;
    }

    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
        perror("Falha ao definir as opções do socket");
        return 1;
    }

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    if (bind(server_fd, (struct sockaddr*)&address, sizeof(address)) < 0) {
        perror("Falha ao fazer o bind");
        return 1;
    }

    if (listen(server_fd, 3) < 0) {
        perror("Falha ao ouvir a porta");
        return 1;
    }

    printf("Aguardando a conexão de Alice...\n");
    if ((new_socket = accept(server_fd, (struct sockaddr*)&address, (socklen_t*)&addrlen)) < 0) {
        perror("Falha ao aceitar a conexão");
        return 1;
    }

    // Receber o arquivo criptografado de Alice
    printf("Recebendo o arquivo criptografado de Alice...\n");
    FILE* file_received = fopen("alice_encrypted.bmp", "wb");
    if (!file_received) {
        perror("Erro ao criar o arquivo de saída");
        return 1;
    }

    char buffer[1024];
    int bytes_received;
    while ((bytes_received = recv(new_socket, buffer, sizeof(buffer), 0)) > 0) {
        fwrite(buffer, 1, bytes_received, file_received);
    }
    fclose(file_received);
    close(new_socket);

    // Descriptografar o arquivo recebido
    printf("Descriptografando o arquivo...\n");
    FILE* encrypted_file = fopen("alice_encrypted.bmp", "rb");
    FILE* decrypted_file = fopen("bob_decrypt.bmp", "wb");
    if (!encrypted_file || !decrypted_file) {
        perror("Erro ao abrir os arquivos");
        return 1;
    }

    unsigned char encrypted[256];
    unsigned char decrypted[256];
    int encrypted_len, decrypted_len;

    while ((encrypted_len = fread(encrypted, 1, sizeof(encrypted), encrypted_file)) > 0) {
        decrypted_len = RSA_private_decrypt(encrypted_len, encrypted, decrypted, rsa_private_key, RSA_PKCS1_PADDING);
        fwrite(decrypted, 1, decrypted_len, decrypted_file);
    }
    fclose(encrypted_file);
    fclose(decrypted_file);

    printf("Arquivo descriptografado com sucesso!\n");

    // Limpar recursos do OpenSSL
    RSA_free(rsa_private_key);
    EVP_cleanup();
    ERR_free_strings();

    return 0;
}
