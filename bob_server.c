#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <netinet/in.h>
#include <unistd.h>

// Função para descriptografar o arquivo BMP usando RSA
int decryptBMP_RSA(const char *inputFileName, const char *outputFileName, const char *privateKeyFileName) {
    FILE *inputFile, *outputFile;
    unsigned char inputBuffer[256];
    unsigned char outputBuffer[128];

    RSA *rsaPrivateKey = RSA_new();

    // Importar a chave privada
    FILE *privateKeyFile = fopen(privateKeyFileName, "rb");
    PEM_read_RSAPrivateKey(privateKeyFile, &rsaPrivateKey, NULL, NULL);
    fclose(privateKeyFile);

    if ((inputFile = fopen(inputFileName, "rb")) == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        RSA_free(rsaPrivateKey);
        return 1;
    }

    if ((outputFile = fopen(outputFileName, "wb")) == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(inputFile);
        RSA_free(rsaPrivateKey);
        return 1;
    }

    int rsaSize = RSA_size(rsaPrivateKey);
    int bytesRead;

    while ((bytesRead = fread(inputBuffer, sizeof(unsigned char), rsaSize, inputFile)) > 0) {
        int decryptedSize = RSA_private_decrypt(bytesRead, inputBuffer, outputBuffer, rsaPrivateKey, RSA_PKCS1_PADDING);
        fwrite(outputBuffer, sizeof(unsigned char), decryptedSize, outputFile);
    }

    if (!feof(inputFile)) {
        perror("Erro durante a leitura do arquivo de entrada");
    }

    fclose(inputFile);
    fclose(outputFile);

    RSA_free(rsaPrivateKey);

    return 0;
}

int main() {
    const char *encryptedFileName = "fractal_encrypted_rsa.bin";
    const char *decryptedFileName = "fractal_decrypted.bmp";
    const char *privateKeyFileName = "private_key.pem";

    // Iniciar o servidor TCP
    int serverSocket, clientSocket;
    struct sockaddr_in serverAddr, clientAddr;
    socklen_t clientAddrLen = sizeof(clientAddr);

    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("Erro ao criar o socket");
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(8888); // Porta do servidor (pode ser qualquer porta disponível)

    if (bind(serverSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro no bind");
        close(serverSocket);
        return 1;
    }

    if (listen(serverSocket, 1) < 0) {
        perror("Erro no listen");
        close(serverSocket);
        return 1;
    }

    printf("Aguardando conexão do cliente...\n");

    // Aceitar conexão do cliente
    clientSocket = accept(serverSocket, (struct sockaddr *)&clientAddr, &clientAddrLen);
    if (clientSocket < 0) {
        perror("Erro ao aceitar conexão");
        close(serverSocket);
        return 1;
    }

    printf("Cliente conectado! Recebendo arquivo...\n");

    // Receber o arquivo BMP criptografado do cliente
    FILE *encryptedFile = fopen(encryptedFileName, "wb");
    if (encryptedFile == NULL) {
        perror("Erro ao abrir o arquivo criptografado");
        close(clientSocket);
        close(serverSocket);
        return 1;
    }

    unsigned char buffer[4096];
    int bytesRead;

    while ((bytesRead = read(clientSocket, buffer, sizeof(buffer))) > 0) {
        fwrite(buffer, sizeof(unsigned char), bytesRead, encryptedFile);
    }

    fclose(encryptedFile);

    printf("Arquivo recebido com sucesso!\n");

    // Descriptografar o arquivo BMP usando RSA
    int result = decryptBMP_RSA(encryptedFileName, decryptedFileName, privateKeyFileName);
    if (result == 0) {
        printf("Arquivo descriptografado com sucesso!\n");
    } else {
        printf("Erro ao descriptografar o arquivo.\n");
    }

    close(clientSocket);
    close(serverSocket);

    return 0;
}