#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <time.h>

// Função para verificar se um número é primo
bool is_prime(int num) {
    if (num <= 1)
        return false;
    if (num <= 3)
        return true;

    if (num % 2 == 0 || num % 3 == 0)
        return false;

    for (int i = 5; i * i <= num; i += 6) {
        if (num % i == 0 || num % (i + 2) == 0)
            return false;
    }

    return true;
}

// Função para gerar números primos aleatórios entre 'min' e 'max'
int generate_prime(int min, int max) {
    int num;
    do {
        num = rand() % (max - min + 1) + min;
    } while (!is_prime(num));
    return num;
}

// Função para calcular o máximo divisor comum (MDC) de dois números
int gcd(int a, int b) {
    if (b == 0)
        return a;
    return gcd(b, a % b);
}

// Função para calcular a chave pública (e) a partir de p e q
int calculate_public_key(int p, int q) {
    int phi = (p - 1) * (q - 1);
    int e = 2;
    while (e < phi) {
        if (gcd(e, phi) == 1)
            break;
        e++;
    }
    return e;
}

// Função para calcular a chave privada (d) a partir de p, q e a chave pública (e)
int calculate_private_key(int p, int q, int e) {
    int phi = (p - 1) * (q - 1);
    int k = 1;
    while ((e * k) % phi != 1 || k == e)
        k++;
    return k;
}

// Função para salvar os primos p e q em um arquivo
void save_primes_to_file(int p, int q) {
    FILE* file = fopen("primos.txt", "w");
    if (file != NULL) {
        fprintf(file, "%d#%d", p, q);
        fclose(file);
    } else {
        printf("Erro ao criar o arquivo primos.txt.\n");
    }
}

// Função para salvar a chave pública (e) em um arquivo
void save_public_key_to_file(int e) {
    FILE* file = fopen("chave.pub", "w");
    if (file != NULL) {
        fprintf(file, "%d", e);
        fclose(file);
    } else {
        printf("Erro ao criar o arquivo chave.pub.\n");
    }
}

// Função para salvar a chave privada (d) em um arquivo
void save_private_key_to_file(int d) {
    FILE* file = fopen("chave.priv", "w");
    if (file != NULL) {
        fprintf(file, "%d", d);
        fclose(file);
    } else {
        printf("Erro ao criar o arquivo chave.priv.\n");
    }
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    int p, q;
    bool generate_primes = false;

    if (argc == 2 && strcmp(argv[1], "-p") == 0) {
        generate_primes = true;
    } else {
        printf("Uso: gerarsa -p\n");
        return 1;
    }

    if (generate_primes) {
        // Gerar números primos aleatórios com cinco e seis dígitos
        p = generate_prime(10000, 99999);
        q = generate_prime(100000, 999999);

        // Salvar os primos em um arquivo
        save_primes_to_file(p, q);
        printf("Primos gerados e salvos em primos.txt.\n");
    }

    printf("Pagora ta indo.txt.\n");
    // Calcular as chaves pública e privada
    int e = calculate_public_key(p, q);
    int d = calculate_private_key(p, q, e);

    // Salvar as chaves em arquivos separados
    save_public_key_to_file(e);
    save_private_key_to_file(d);

    printf("Chaves pública e privada geradas e salvas em chave.pub e chave.priv.\n");

    return 0;
}