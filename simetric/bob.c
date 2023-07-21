#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <openssl/des.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080

void die(const char *msg)
{
    perror(msg);
    exit(1);
}

void decrypt_data(const char *input_data, int data_size, const char *key, char *output_data)
{
    DES_cblock des_key;
    memcpy(des_key, key, 8);

    DES_key_schedule key_schedule;
    DES_set_key(&des_key, &key_schedule);

    int remaining_bytes = data_size;
    unsigned char buffer[8];

    while (remaining_bytes > 0)
    {
        int bytes_to_decrypt = (remaining_bytes >= 8) ? 8 : remaining_bytes;
        memcpy(buffer, input_data, bytes_to_decrypt);
        DES_ecb_encrypt((const_DES_cblock *)buffer, (DES_cblock *)buffer, &key_schedule, DES_DECRYPT);
        memcpy(output_data, buffer, bytes_to_decrypt);
        input_data += bytes_to_decrypt;
        output_data += bytes_to_decrypt;
        remaining_bytes -= bytes_to_decrypt;
    }
}

int main()
{
    int client_socket;
    struct sockaddr_in server_addr;
    DES_cblock key;

    // Cria o socket TCP
    client_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (client_socket < 0)
        die("Erro ao criar socket");

    // Configura o endereço do servidor
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(PORT);
    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
        die("Erro ao configurar o endereço do servidor");

    // Conecta-se ao servidor
    if (connect(client_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0)
        die("Erro ao conectar ao servidor");

    printf("Conectado ao servidor.\n");

    // Recebe a chave do servidor (Bob)
    if (recv(client_socket, key, 8, 0) != 8)
        die("Erro ao receber a chave do servidor");

    printf("Chave recebida de Alice.\n");

    // Recebe a imagem encriptada
    char buffer[1024];
    int total_bytes_received = 0;
    FILE *output_file = fopen("encrypted_received.bmp", "wb");
    if (!output_file)
        die("Erro ao criar o arquivo de saída descriptografado");

    int bytes_received;
    while ((bytes_received = recv(client_socket, buffer, sizeof(buffer), 0)) > 0)
    {
        // Escreve os dados recebidos no arquivo
        fwrite(buffer, 1, bytes_received, output_file);
        total_bytes_received += bytes_received;
    }

    fclose(output_file);
    printf("Imagem encriptada recebida de Alice. Total de bytes recebidos: %d\n", total_bytes_received);

    // Descriptografa a imagem encriptada
    FILE *encrypted_file = fopen("encrypted_received.bmp", "rb");
    if (!encrypted_file)
        die("Erro ao abrir o arquivo de imagem encriptada");

    fseek(encrypted_file, 0L, SEEK_END);
    int encrypted_file_size = ftell(encrypted_file);
    fseek(encrypted_file, 0L, SEEK_SET);

    unsigned char *encrypted_data = (unsigned char *)malloc(encrypted_file_size);
    fread(encrypted_data, 1, encrypted_file_size, encrypted_file);
    fclose(encrypted_file);

    // Descriptografa o corpo do arquivo BMP
    unsigned char *decrypted_body = (unsigned char *)malloc(encrypted_file_size - 54);
    decrypt_data(encrypted_data + 54, encrypted_file_size - 54, (const char *)key, (char *)decrypted_body);
    free(encrypted_data);

    // Escreve o corpo descriptografado em um novo arquivo BMP
    FILE *decrypted_file = fopen("decrypted_image.bmp", "wb");
    if (!decrypted_file)
        die("Erro ao criar o arquivo de imagem descriptografada");

    // Escreve o cabeçalho original no arquivo descriptografado
    FILE *header_file = fopen("encrypted_received.bmp", "rb");
    if (!header_file)
        die("Erro ao abrir o arquivo de imagem encriptada para ler o cabeçalho");

    unsigned char header[54];
    fread(header, 1, sizeof(header), header_file);
    fclose(header_file);

    fwrite(header, 1, sizeof(header), decrypted_file);
    fwrite(decrypted_body, 1, encrypted_file_size - 54, decrypted_file);
    fclose(decrypted_file);
    free(decrypted_body);

    printf("Imagem descriptografada salva em decrypted_image.bmp\n");

    // Fecha o socket do cliente
    close(client_socket);

    return 0;
}
