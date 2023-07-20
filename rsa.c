#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>

// Função para gerar chaves pública e privada RSA
void generateRSAKeys(const char *publicKeyFileName, const char *privateKeyFileName, int keySize) {
    RSA *rsa = RSA_generate_key(keySize, RSA_F4, NULL, NULL);

    // Exportar chave pública
    FILE *publicKeyFile = fopen(publicKeyFileName, "wb");
    PEM_write_RSAPublicKey(publicKeyFile, rsa);
    fclose(publicKeyFile);

    // Exportar chave privada
    FILE *privateKeyFile = fopen(privateKeyFileName, "wb");
    PEM_write_RSAPrivateKey(privateKeyFile, rsa, NULL, NULL, 0, NULL, NULL);
    fclose(privateKeyFile);

    RSA_free(rsa);
}

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
    const char *inputFileName = "fractaljulia.bmp";
    const char *encryptedFileName = "fractal_encrypted_rsa.bin";
    const char *publicKeyFileName = "public_key.pem";
    int keySize = 2048; // Tamanho da chave em bits (pode ser 2048 ou 4096)

    // Gerar chaves pública e privada RSA
    generateRSAKeys(publicKeyFileName, "private_key.pem", keySize);

    // Criptografar o arquivo BMP usando RSA
    int result = encryptBMP_RSA(inputFileName, encryptedFileName, publicKeyFileName);
    if (result == 0) {
        printf("Arquivo criptografado com sucesso usando RSA!\n");
    } else {
        printf("Erro ao criptografar o arquivo usando RSA.\n");
    }

    return 0;
}