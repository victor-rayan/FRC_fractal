#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <openssl/err.h>

#define MIN_PRIME_DIGITS 5
#define MAX_PRIME_DIGITS 6

// Função para verificar se um número é primo
bool is_prime(unsigned long n) {
    if (n < 2) return false;
    if (n == 2) return true;

    for (unsigned long i = 2; i * i <= n; i++) {
        if (n % i == 0) return false;
    }

    return true;
}

// Função para gerar um número primo aleatório entre min_digits e max_digits
unsigned long generate_prime(int min_digits, int max_digits) {
    srand(time(NULL));
    unsigned long prime;

    do {
        prime = rand() % (max_digits * 10) + (min_digits * 10);
    } while (!is_prime(prime));

    return prime;
}

// Função para salvar a chave pública em um arquivo
void save_public_key(RSA *rsa, const char *filename) {
    FILE *fp = fopen(filename, "w");
    PEM_write_RSAPublicKey(fp, rsa);
    fclose(fp);
}

// Função para salvar a chave privada em um arquivo
void save_private_key(RSA *rsa, const char *filename) {
    FILE *fp = fopen(filename, "w");
    PEM_write_RSAPrivateKey(fp, rsa, NULL, NULL, 0, NULL, NULL);
    fclose(fp);
}

// Função para carregar a chave privada de um arquivo
RSA *load_private_key(const char *filename) {
    FILE *fp = fopen(filename, "r");
    RSA *rsa = PEM_read_RSAPrivateKey(fp, NULL, NULL, NULL);
    fclose(fp);
    return rsa;
}

// Função para encriptar dados usando a chave pública
int encrypt_data(const char *input_file, const char *output_file, RSA *rsa) {
    FILE *fin = fopen(input_file, "rb");
    FILE *fout = fopen(output_file, "wb");

    if (fin == NULL || fout == NULL) {
        return 1;
    }

    unsigned char in[256], out[256];
    int len;

    while ((len = fread(in, sizeof(unsigned char), sizeof(in), fin)) > 0) {
        int outlen = RSA_public_encrypt(len, in, out, rsa, RSA_PKCS1_PADDING);
        fwrite(out, sizeof(unsigned char), outlen, fout);
    }

    fclose(fin);
    fclose(fout);

    return 0;
}

// Função para desencriptar dados usando a chave privada
int decrypt_data(const char *input_file, const char *output_file, RSA *rsa) {
    FILE *fin = fopen(input_file, "rb");
    FILE *fout = fopen(output_file, "wb");

    if (fin == NULL || fout == NULL) {
        return 1;
    }

    unsigned char in[256], out[256];
    int len;

    while ((len = fread(in, sizeof(unsigned char), sizeof(in), fin)) > 0) {
        int outlen = RSA_private_decrypt(len, in, out, rsa, RSA_PKCS1_PADDING);
        fwrite(out, sizeof(unsigned char), outlen, fout);
    }

    fclose(fin);
    fclose(fout);

    return 0;
}

int main(int argc, char *argv[]) {
    if (argc != 2) {
        printf("Uso: %s -p | -q | -keys | -encrypt | -decrypt\n", argv[0]);
        return 1;
    }

    char *param = argv[1];

    if (strcmp(param, "-p") == 0) {
        unsigned long p = generate_prime(MIN_PRIME_DIGITS, MAX_PRIME_DIGITS);
        printf("p = %lu\n", p);
        return 0;
    } else if (strcmp(param, "-q") == 0) {
        unsigned long q = generate_prime(MIN_PRIME_DIGITS, MAX_PRIME_DIGITS);
        printf("q = %lu\n", q);
        return 0;
    } else if (strcmp(param, "-keys") == 0) {
        unsigned long p = generate_prime(MIN_PRIME_DIGITS, MAX_PRIME_DIGITS);
        unsigned long q = generate_prime(MIN_PRIME_DIGITS, MAX_PRIME_DIGITS);

        RSA *rsa = RSA_new();
        BIGNUM *e = BN_new();
        BN_set_word(e, RSA_F4);

        if (RSA_generate_key_ex(rsa, 2048, e, NULL) != 1) {
            printf("Erro ao gerar as chaves.\n");
            return 1;
        }

        save_public_key(rsa, "chave.pub");
        save_private_key(rsa, "chave.priv");

        RSA_free(rsa);
        BN_free(e);

        printf("Chaves públicas e privadas geradas com sucesso.\n");
        return 0;
    } else if (strcmp(param, "-encrypt") == 0) {
        RSA *rsa = load_private_key("chave.priv");
        if (rsa == NULL) {
            printf("Erro ao carregar a chave privada.\n");
            return 1;
        }

        if (encrypt_data("input.txt", "encrypted_output.txt", rsa) != 0) {
            printf("Erro ao encriptar os dados.\n");
            RSA_free(rsa);
            return 1;
        }

        RSA_free(rsa);
        printf("Dados encriptados com sucesso.\n");
        return 0;
    } else if (strcmp(param, "-decrypt") == 0) {
        RSA *rsa = load_private_key("chave.priv");
        if (rsa == NULL) {
            printf("Erro ao carregar a chave privada.\n");
            return 1;
        }

        if (decrypt_data("encrypted_output.txt", "decrypted_output.txt", rsa) != 0) {
            printf("Erro ao desencriptar os dados.\n");
            RSA_free(rsa);
            return 1;
        }

        RSA_free(rsa);
        printf("Dados desencriptados com sucesso.\n");
        return 0;
    } else {
        printf("Parâmetro inválido. Uso: %s -p | -q | -keys | -encrypt | -decrypt\n", argv[0]);
        return 1;
    }
}
