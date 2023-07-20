# FRC_fractal

Claro, vamos fornecer um overview geral de todo o projeto, explicando o fluxo completo desde o conceito do fractal até a comunicação segura entre Alice e Bob usando criptografia RSA.

Overview Geral do Projeto

Fractal Julia

O projeto começa com um código em C chamado fractal.c, que gera o fractal Julia. O fractal é uma imagem complexa gerada por meio de iterações de uma fórmula específica, que leva a diferentes imagens dependendo dos valores utilizados. O fractal Julia é definido por uma série de cálculos matemáticos envolvendo um número complexo e um valor fixo "c" (-0.79 + i * 0.15 no caso deste projeto).

O código fractal.c gera o fractal Julia e salva a imagem gerada em formato BMP em um arquivo. É possível alterar os valores para "c" para obter diferentes imagens do fractal.

Criptografia Simétrica (DES) - Parte 1
Em seguida, adicionamos a funcionalidade de criptografia simétrica (DES) em um novo arquivo chamado encrypt_fractal.c. O DES é um algoritmo de criptografia simétrica amplamente utilizado para proteger dados.

O programa encrypt_fractal.c lê o arquivo BMP gerado pelo código fractal.c, criptografa o corpo do arquivo BMP usando DES e salva o resultado em um novo arquivo binário criptografado. A chave para criptografar e descriptografar o arquivo é fixa no código ("secretpwd" no exemplo).

Criptografia Assimétrica (RSA) - Parte 2
Em seguida, implementamos a criptografia assimétrica (RSA) em um novo arquivo chamado encrypt_fractal_rsa.c. O RSA é um algoritmo de criptografia assimétrica que utiliza um par de chaves, uma pública e outra privada.

O programa encrypt_fractal_rsa.c gera um par de chaves RSA (pública e privada), salva-os em arquivos, e criptografa o arquivo BMP usando a chave pública. O arquivo BMP criptografado é salvo em um novo arquivo binário.

Comunicação Segura entre Alice e Bob usando RSA
Agora, temos dois programas independentes: bob_server.c (Bob) e alice_client.c (Alice).

Bob é o servidor, que aguarda conexões de clientes e realiza a descriptografia do arquivo BMP recebido usando a chave privada RSA.

Alice é o cliente, que criptografa o arquivo BMP usando a chave pública RSA de Bob e envia-o para o servidor.

Fluxo Completo do Projeto
Primeiro, compila-se todos os arquivos separadamente:

gcc -o fractal fractal.c -lm
gcc -o encrypt_fractal encrypt_fractal.c -lcrypto
gcc -o encrypt_fractal_rsa encrypt_fractal_rsa.c -lcrypto
gcc -o bob_server bob_server.c -lcrypto
gcc -o alice_client alice_client.c -lcrypto

Executa-se o programa fractal para gerar o fractal Julia e salvar a imagem BMP:


./fractal <N>
Em seguida, executa-se o programa encrypt_fractal para criptografar o arquivo BMP gerado anteriormente usando DES:


./encrypt_fractal
O programa encrypt_fractal_rsa gera o par de chaves RSA (pública e privada) e criptografa o arquivo BMP usando a chave pública:


./encrypt_fractal_rsa
Por fim, inicia-se o servidor (Bob) para aguardar a conexão do cliente:
bash

./bob_server
E em outro terminal, executa-se o cliente (Alice) para enviar o arquivo BMP criptografado para o servidor:
bash

./alice_client
O servidor descriptografa o arquivo BMP recebido usando sua chave privada RSA e salva o resultado em fractal_decrypted.bmp