#include "common.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include <sys/socket.h>
#include <sys/types.h>

#define BUFSZ 1024
#define POKE_LIMIT 40
#define NAME_LIMIT 50

// Lista encadeada de strings + funções básicas
typedef struct list{
    char name[NAME_LIMIT];
    struct list* next;
}list;

int has_element(list* head, char* name){
    if (!head) return 0;
    list* aux = head;
    while(aux != NULL) {
        if (strcmp(aux->name, name) == 0) return 1;
        aux = aux->next;
    }
    return 0;
}

list* insert_element(list* head, char* name){
    if (!head) {
        head = (list*) malloc(sizeof(list));
        strcpy(head->name, name);
        head->next = NULL;
        return head;
    }
    list* aux = head;
    list* new;
    while(aux->next != NULL){
        aux = aux->next;
    }
    new = (list*) malloc(sizeof(list)); 
    strcpy(new->name, name);
    new->next = NULL;
    aux->next = new;
    return head;
}

list* remove_element(list* head, char* name){
    list* aux = head;
    list* temp = NULL;
    if (strcmp(aux->name, name) == 0){
        if (aux->next == NULL) {
            head = NULL;
            return head;
        }
        else{
            temp = head;
            head = head->next;
            free(temp);
        }
        return head;
    }
    while(aux != NULL){
        if (strcmp(aux->next->name, name) == 0){
            temp = aux->next;
            aux->next = temp->next;
            free(temp);
            return head;
        }
        aux = aux->next;
    }
    return head;
}

list* swap_element(list* head, char* name, char* new_name){
    list* aux = head;
    while(aux != NULL){
        if (strcmp(aux->name, name) == 0){
            strcpy(aux->name, new_name);
            return head;
        }
        aux = aux->next;
    }
    return head;
}

// Retorna lista de pokémons existentes na estrutura
char* get_list(list* head){
    if (!head) return "none";
    list* aux = head;
    char* list = (char*) malloc(BUFSZ), name[NAME_LIMIT];
    while(aux != NULL){
        strcpy(name, aux->name);
        strcat(name, " ");
        strcat(list, name);
        aux = aux->next;
    }
    list[strlen(list)-1] = '\0';
    return list;
}


// Função para verificar se nome de pokemon é válido
int is_word_valid(char* word){
    int n = strlen(word);
    if (n > 10) return 0;
    for (int i = 0; i < n; i++){
        if(ispunct(word[i]) || isupper(word[i])) return 0;
    }
    return 1;
}


// Pokedex possui lista de pokemons e número de pokemons
typedef struct Pokedex{
    list* pokemons;
    int num_poke;
}Pokedex;

Pokedex* init_pokedex(){
    Pokedex* pokedex = (Pokedex*) malloc(sizeof(Pokedex));
    pokedex->pokemons = NULL;
    pokedex->num_poke = 0;
    return pokedex;
}

// Acões possíveis: add, remove, list, exchange e kill
char* access_pokedex(Pokedex** pokedex, char* action){
    action[strcspn(action, "\n")] = 0;
    char* command = strtok(action, " "), *poke, *rsp = (char*) malloc(BUFSZ);

    // Comando add
    if (strcmp(command, "add") == 0){
        char name[NAME_LIMIT];
        poke = strtok(NULL, " ");
        while(poke){
            if(!is_word_valid(poke)) strcat(rsp, "invalid message ");
            else if ( (*pokedex)->num_poke == POKE_LIMIT ){
                strcat(rsp, "limit exceeded ");
            }
            else if (!has_element((*pokedex)->pokemons, poke)){
                (*pokedex)->pokemons = insert_element((*pokedex)->pokemons, poke);
                sprintf(name, "%s added ", poke);
                strcat(rsp, name);
                (*pokedex)->num_poke++;
            }
            else{
                sprintf(name, "%s already exists ", poke);
                strcat(rsp, name);
            }
            poke = strtok(NULL, " ");
        }
        rsp[strlen(rsp)-1] = '\0';
        return rsp;
    }

    // Comando remove
    else if (strcmp(command, "remove") == 0){
        poke = strtok(NULL, " ");
        if(!is_word_valid(poke)) return "invalid message";
        if (!has_element((*pokedex)->pokemons, poke)){
            sprintf(rsp, "%s does not exist", poke);
            return rsp;
        }
        else {
            (*pokedex)->pokemons = remove_element((*pokedex)->pokemons, poke);
            (*pokedex)->num_poke--;
            sprintf(rsp, "%s removed", poke);
            return rsp;
        }
    }

    // Comando list
    else if (strcmp(command, "list") == 0){
        rsp = get_list((*pokedex)->pokemons);
        return rsp;
    }

    // Comando exchange
    else if (strcmp(command, "exchange") == 0){
        poke = strtok(NULL, " ");
        if(!is_word_valid(poke)) return "invalid message";
        if (!has_element((*pokedex)->pokemons, poke)){
            sprintf(rsp, "%s does not exist", poke);
            return rsp;
        }
        char* poke2 = strtok(NULL, " ");
        if(!is_word_valid(poke2)) return "invalid message";
        if (has_element((*pokedex)->pokemons, poke2)){
            sprintf(rsp, "%s already exists", poke2);
            return rsp;
        }
        (*pokedex)->pokemons = swap_element((*pokedex)->pokemons, poke, poke2);
        sprintf(rsp, "%s exchanged", poke);
        return rsp;
    }

    // Comando kill
    else if (strcmp(command, "kill") == 0){
        return "kill";
    }

    else {
        return "invalid";
    }
}


int main(int argc, char **argv) {
    if (argc < 3) {
        exit(EXIT_FAILURE);
    }

    struct sockaddr_storage storage;
    if (0 != server_sockaddr_init(argv[1], argv[2], &storage)) {
        exit(EXIT_FAILURE);
    }

    int s;
    s = socket(storage.ss_family, SOCK_STREAM, 0);
    if (s == -1) {
        logexit("socket");
    }

    int enable = 1;
    if (0 != setsockopt(s, SOL_SOCKET, SO_REUSEADDR, &enable, sizeof(int))) {
        logexit("setsockopt");
    }

    struct sockaddr *addr = (struct sockaddr *)(&storage);
    if (0 != bind(s, addr, sizeof(storage))) {
        logexit("bind");
    }

    if (0 != listen(s, 10)) {
        logexit("listen");
    }

    int flag;  // Usado para verificar se cliente caiu
    while(1){
        flag = 0; 

        printf("waiting connections\n");

        struct sockaddr_storage cstorage;
        struct sockaddr *caddr = (struct sockaddr *)(&cstorage);
        socklen_t caddrlen = sizeof(cstorage);

        int csock = accept(s, caddr, &caddrlen);
        if (csock == -1) {
            logexit("accept");
        }

        printf("[log] connected\n");

        Pokedex* pokedex = init_pokedex();
        char* rsp;
        size_t count;
        while (1) {
            char buf[BUFSZ], aux[NAME_LIMIT];
            memset(buf, 0, BUFSZ);

            // Chama recv até que tenha um '\n' na mensagem
            while (1){
                memset(aux, 0, NAME_LIMIT);
                count = recv(csock, aux, NAME_LIMIT - 1, 0);
                if (count == 0 || count == -1){
                    close(csock);
                    flag = 1;
                    break;
                }
                strcat(buf, aux);
                if (aux[strlen(aux)-1] == '\n') break;
            }
            if (flag == 1) break;

            printf("[msg]: %s\n", buf);

            rsp = access_pokedex(&pokedex, buf);

            if (strcmp(rsp, "kill") == 0){
                free(pokedex->pokemons);
                free(pokedex);
                close(csock);
                exit(EXIT_SUCCESS);
            } else if (strcmp(rsp, "invalid") == 0){
                free(pokedex->pokemons);
                free(pokedex);
                close(csock);
                break;
            }

            sprintf(buf, "%s\n", rsp);
            count = send(csock, buf, strlen(buf), 0);
            if (count != strlen(buf)) {
                logexit("send");
            }
        }
    }

    exit(EXIT_SUCCESS);
}