#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/des.h>

#define PORT 8080

void die(const char *msg)
{
    perror(msg);
    exit(1);
}

void encrypt_data(const char *input_data, int data_size, const char *key, char *output_data)
{
    DES_cblock des_key;
    memcpy(des_key, key, 8);

    DES_key_schedule key_schedule;
    DES_set_key(&des_key, &key_schedule);

    int remaining_bytes = data_size;
    unsigned char buffer[8];

    while (remaining_bytes > 0)
    {
        int bytes_to_encrypt = (remaining_bytes >= 8) ? 8 : remaining_bytes;
        memcpy(buffer, input_data, bytes_to_encrypt);
        DES_ecb_encrypt((const_DES_cblock *)buffer, (DES_cblock *)buffer, &key_schedule, DES_ENCRYPT);
        memcpy(output_data, buffer, bytes_to_encrypt);
        input_data += bytes_to_encrypt;
        output_data += bytes_to_encrypt;
        remaining_bytes -= bytes_to_encrypt;
    }
}

int main()
{
    int server_socket, client_socket;
    struct sockaddr_in server_addr, client_addr;
    socklen_t addr_size;
    DES_cblock key;

    // Chave DES de 7 bytes (56 bits)
    const char *key_data = "labrede";
    memcpy(key, key_data, 8);

    // Cria o socket TCP
    server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
        die("Erro ao criar socket");

    // Configura o endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    server_addr.sin_addr.s_addr = INADDR_ANY;

    // Faz o bind do socket com o endereço
    if (bind(server_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        die("Erro ao fazer o bind");

    // Configura o servidor para escutar conexões
    listen(server_socket, 1);
    printf("Aguardando conexão...\n");

    // Aceita a conexão com o cliente
    addr_size = sizeof(client_addr);
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr, &addr_size);
    if (client_socket < 0)
        die("Erro ao aceitar conexão");

    printf("Conexão estabelecida com %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Envia a chave para o cliente (Bob)
    if (send(client_socket, key, 8, 0) != 8)
        die("Erro ao enviar a chave para o cliente");

    printf("Chave enviada para Bob.\n");

    // Lê o arquivo .bmp e encripta somente o corpo
    FILE *fractalJulia_file = fopen("fractaljulia.bmp", "rb");
    if (!fractalJulia_file)
        die("Erro ao abrir o arquivo BMP");

    fseek(fractalJulia_file, 0L, SEEK_END);
    int file_size = ftell(fractalJulia_file);
    fseek(fractalJulia_file, 0L, SEEK_SET);

    // Lê o cabeçalho
    unsigned char header[54];
    fread(header, 1, sizeof(header), fractalJulia_file);

    // Lê o corpo do arquivo BMP
    int body_size = file_size - sizeof(header);
    unsigned char *body_data = (unsigned char *)malloc(body_size);
    fread(body_data, 1, body_size, fractalJulia_file);
    fclose(fractalJulia_file);

    // Criptografa o corpo do arquivo BMP
    unsigned char *encrypted_body = (unsigned char *)malloc(body_size);
    encrypt_data(body_data, body_size, key_data, encrypted_body);
    free(body_data);

    // Escreve o arquivo BMP criptografado
    FILE *fractal_julia_encrypted_file = fopen("encrypted.bmp", "wb");
    if (!fractal_julia_encrypted_file)
        die("Erro ao criar o arquivo de saída criptografado");

    fwrite(header, 1, sizeof(header), fractal_julia_encrypted_file);
    fwrite(encrypted_body, 1, body_size, fractal_julia_encrypted_file);
    fclose(fractal_julia_encrypted_file);
    free(encrypted_body);

    // Lê o arquivo criptografado e envia para Bob
    FILE *encrypted_file = fopen("encrypted.bmp", "rb");
    if (!encrypted_file)
        die("Erro ao abrir o arquivo criptografado");

    fseek(encrypted_file, 0L, SEEK_END);
    int encrypted_file_size = ftell(encrypted_file);
    fseek(encrypted_file, 0L, SEEK_SET);

    char *encrypted_data = (char *)malloc(encrypted_file_size);
    fread(encrypted_data, 1, encrypted_file_size, encrypted_file);
    fclose(encrypted_file);

    send(client_socket, encrypted_data, encrypted_file_size, 0);
    free(encrypted_data);

    // Fecha o socket do cliente
    close(client_socket);

    return 0;
}
