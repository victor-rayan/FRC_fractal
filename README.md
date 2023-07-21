# FRC - Laboratório Criptografia

Laboratório de criptografia da matéria de Fundamentos de Redes de Computadores.

## Simetric

Nesta pasta temos o servidor (Alice) e o cliente (Bob) reponsáveis por enviar uma imagem do fractal julia (gerado pelo código fractal.c) utilizando o algoritmo de criptografia simétrica DES.

### Como rodar

Siga os seguintes passos na ordem em que aparecem abaixo:

```
$ cd simetric

```

#### Gerando o fractal

```
$ gcc -o fractal fractal.c -lm

$ ./fractal 200
```

#### Iniciando o servidor

```
$ gcc -o server alice.c -lssl -lcrypto

$ ./server
```

#### Iniciando o cliente

```
$ gcc -o client bob.c -lssl -lcrypto

$  ./client

```

## Assimetrica

Nesta pasta temos o servidor (Alice) e o cliente (Bob) reponsáveis por enviar uma imagem do fractal julia (gerado pelo código fractal.c) utilizando o algoritmo de criptografia assimétrica RSA.

### Como rodar
