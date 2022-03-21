# TCP Pokédex
O objetivo deste trabalho prático foi assimilar e exercitar os conceitos sobre programação com sockets e protocolo de comunicação TCP vistos na disciplina Redes de Computadores. O servidor funciona como uma Pokédex, que pode armazenar, excluir e fazer trocas de Pokémons para o treinador, que é o cliente.

A seguir, são apresentadas as instruções de execução e as operações suportadas pelo servidor.

## Instruções

**Passo 1:** Rode o comando `make` para compilação.

**Passo 2:** Inicie o servidor passando como parâmetro o `IP` (**v4** para IPV4 ou **v6** para IPV6) e a `porta`. Como por exemplo:

```bash
./server v4 5151
```

**Passo 3:** Inicie o cliente passando como parâmentro o `endereço` (**127.0.0.1** para IPV4 ou **::1** para IPV6) e a `porta do servidor`. Para o servidor iniciado no Passo 2, por exemplo, executa-se o cliente com o seguinte comando:

```bash
./client 127.0.0.1 5151
```

## Operações

As operações suportadas pelo servidor e enviadas pelo cliente são as seguintes: 
* **add:** é seguido de uma string contendo o nome do pokémon e faz o servidor salvá-lo na Pokédex.
* **remove:** é seguido de uma string contendo o nome do pokémon e faz o servidor removê-lo da Pokédex.
* **exchange:** é seguido de duas strings: a primeira contém o nome do pokémon a ser trocado e a segunda contém o nome do novo pokémon. Ao receber o comando, o servidor realiza a troca.
* **list:** mostra a lista de todos os pokémons salvos na Pokédex.
* **kill:** encerra a execução tanto do cliente quanto do servidor.

## Exemplo de Execução

<p align="center">
  <img src="https://user-images.githubusercontent.com/42253628/159305057-db7bce89-629c-4e7a-92f0-23e329adb152.png" />
  <img src="https://user-images.githubusercontent.com/42253628/159305557-05399ab4-8426-4c5b-aed1-36a34d94be75.png" />
</p>

