#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <openssl/des.h>

// Função para criptografar o arquivo BMP usando DES
int encryptBMP(const char *inputFileName, const char *outputFileName, const char *key) {
    FILE *inputFile, *outputFile;
    unsigned char inputBuffer[8];
    unsigned char outputBuffer[8];

    DES_key_schedule keySchedule;
    DES_cblock desKey;
    DES_string_to_key(key, &desKey);

    if ((inputFile = fopen(inputFileName, "rb")) == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    if ((outputFile = fopen(outputFileName, "wb")) == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(inputFile);
        return 1;
    }

    DES_set_key_unchecked(&desKey, &keySchedule);

    while (fread(inputBuffer, sizeof(unsigned char), 8, inputFile) == 8) {
        DES_ecb_encrypt((DES_cblock *)inputBuffer, (DES_cblock *)outputBuffer, &keySchedule, DES_ENCRYPT);
        fwrite(outputBuffer, sizeof(unsigned char), 8, outputFile);
    }

    if (!feof(inputFile)) {
        perror("Erro durante a leitura do arquivo de entrada");
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}

// Função para descriptografar o arquivo BMP usando DES
int decryptBMP(const char *inputFileName, const char *outputFileName, const char *key) {
    FILE *inputFile, *outputFile;
    unsigned char inputBuffer[8];
    unsigned char outputBuffer[8];

    DES_key_schedule keySchedule;
    DES_cblock desKey;
    DES_string_to_key(key, &desKey);

    if ((inputFile = fopen(inputFileName, "rb")) == NULL) {
        perror("Erro ao abrir o arquivo de entrada");
        return 1;
    }

    if ((outputFile = fopen(outputFileName, "wb")) == NULL) {
        perror("Erro ao abrir o arquivo de saída");
        fclose(inputFile);
        return 1;
    }

    DES_set_key_unchecked(&desKey, &keySchedule);

    while (fread(inputBuffer, sizeof(unsigned char), 8, inputFile) == 8) {
        DES_ecb_encrypt((DES_cblock *)inputBuffer, (DES_cblock *)outputBuffer, &keySchedule, DES_DECRYPT);
        fwrite(outputBuffer, sizeof(unsigned char), 8, outputFile);
    }

    if (!feof(inputFile)) {
        perror("Erro durante a leitura do arquivo de entrada");
    }

    fclose(inputFile);
    fclose(outputFile);

    return 0;
}

int main() {
    const char *inputFileName = "fractaljulia.bmp";
    const char *encryptedFileName = "fractal_encrypted.bmp";
    const char *decryptedFileName = "fractal_decrypted.bmp";
    const char *key = "secretpwd"; // Substitua por uma chave segura

    // Criptografar o arquivo BMP
    int result = encryptBMP(inputFileName, encryptedFileName, key);
    if (result == 0) {
        printf("Arquivo criptografado com sucesso!\n");
    } else {
        printf("Erro ao criptografar o arquivo.\n");
    }

    // Descriptografar o arquivo BMP
    result = decryptBMP(encryptedFileName, decryptedFileName, key);
    if (result == 0) {
        printf("Arquivo descriptografado com sucesso!\n");
    } else {
        printf("Erro ao descriptografar o arquivo.\n");
    }

    return 0;
}