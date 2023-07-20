#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>

// Função para criptografar o arquivo BMP usando RSA
int encryptBMP_RSA(const char *inputFileName, const char *outputFileName, const char *publicKeyFileName) {
    FILE *inputFile, *outputFile;
    unsigned char inputBuffer[128];
    unsigned char outputBuffer[256];

    RSA *rsaPublicKey = RSA_new();

    // Importar a chave pública
    FILE *publicKeyFile = fopen(publicKeyFileName, "rb");
    PEM_read_RSAPublicKey(publicKeyFile, &rsaPublicKey, NULL, NULL);
    fclose(publicKeyFile);

    if ((inputFile = fopen(inputFileName, "rb")) == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        RSA_free(rsaPublicKey);
        return 1;
    }

    if ((outputFile = fopen(outputFileName, "wb")) == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(inputFile);
        RSA_free(rsaPublicKey);
        return 1;
    }

    int rsaSize = RSA_size(rsaPublicKey);
    int bytesRead;

    while ((bytesRead = fread(inputBuffer, sizeof(unsigned char), rsaSize - 11, inputFile)) > 0) {
        int encryptedSize = RSA_public_encrypt(bytesRead, inputBuffer, outputBuffer, rsaPublicKey, RSA_PKCS1_PADDING);
        fwrite(outputBuffer, sizeof(unsigned char), encryptedSize, outputFile);
    }

    if (!feof(inputFile)) {
        perror("Erro durante a leitura do arquivo de entrada");
    }

    fclose(inputFile);
    fclose(outputFile);

    RSA_free(rsaPublicKey);

    return 0;
}

int main() {
    const char *encryptedFileName = "fractal_encrypted_rsa.bin";
    const char *publicKeyFileName = "public_key.pem";
    const char *serverIP = "127.0.0.1"; // IP do servidor (Bob)
    int serverPort = 8888; // Porta do servidor (a mesma porta que usamos no lado de Bob)

    // Criptografar o arquivo BMP usando RSA
    int result = encryptBMP_RSA("fractaljulia.bmp", encryptedFileName, publicKeyFileName);
    if (result != 0) {
        printf("Erro ao criptografar o arquivo usando RSA.\n");
        return 1;
    }

    // Iniciar o cliente TCP
    int clientSocket;
    struct sockaddr_in serverAddr;

    clientSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (clientSocket < 0) {
        perror("Erro ao criar o socket");
        return 1;
    }

    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = inet_addr(serverIP);
    serverAddr.sin_port = htons(serverPort);

    if (connect(clientSocket, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
        perror("Erro na conexão");
        close(clientSocket);
        return 1;
    }

    printf("Conexão estabelecida com o servidor! Enviando arquivo...\n");

    // Enviar o arquivo BMP criptografado para o servidor
    FILE *encryptedFile = fopen(encryptedFileName, "rb");
    if (encryptedFile == NULL) {
        perror("Erro ao abrir o arquivo criptografado");
        close(clientSocket);
        return 1;
    }

    unsigned char buffer[4096];
    int bytesRead;

    while ((bytesRead = fread(buffer, sizeof(unsigned char), sizeof(buffer), encryptedFile)) > 0) {
        write(clientSocket, buffer, bytesRead);
    }

    fclose(encryptedFile);

    printf("Arquivo enviado com sucesso!\n");

    close(clientSocket);

    return 0;
}