#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//gcc nomesobrenome.c -o nomesobrenome
//./nomesobrenome input.txt output.txt

typedef struct No{
char nome[50];
struct No* proximo;
struct No* anterior;
} No;

typedef struct {
No* inicio;
No* fim;
} Lista;

void modificar_nome(char* texto){
for(int i = 0; i < strlen(texto); i++){
    if(texto[i] == ' '){
        texto[i] = '_';
}
}
}

void liberar_memoria(Lista* lista){
No* atual = lista->inicio;
while(atual != NULL){
    No* temp = atual;
    atual = atual->proximo;
    free(temp);
}
lista->inicio = NULL;
lista->fim = NULL;
}

void inserir_final(Lista* lista, char* pessoa, FILE* output){
No* atual = lista->inicio;

while(atual != NULL) {
        if (strcmp(atual->nome, pessoa) == 0) {
            char nome_temp[50];
            strcpy(nome_temp, pessoa);
            modificar_nome(nome_temp);
            fprintf(output, "[FAILURE]ADD=%s\n", nome_temp);
            return;
        }
        atual = atual->proximo;
    }

No* novo = (No*)malloc(sizeof(No));
if(novo == NULL){
        printf("Erro ao alocar memória.\n");
        exit(1);
}

if(strlen(pessoa) > 0 && strlen(pessoa) <= 49){
strcpy(novo->nome, pessoa);
novo->proximo = NULL;
novo->anterior = lista->fim;

if (lista->fim != NULL) lista->fim->proximo = novo; //Conecta o antigo último nó ao novo.
else lista->inicio = novo; //só ocorre se a lista estivesse vazia

lista->fim = novo; //aqui atualiza o fim da lista independente se ela estava vazia ou não
char nome_temp[50];
strcpy(nome_temp, pessoa);
modificar_nome(nome_temp);
fprintf(output, "[SUCCESS]ADD=%s\n", nome_temp);
}
else printf("Voce colocou um nome com mais de 50 caracteres\n");
}

void remover_pessoa(Lista* lista, char* pessoa, FILE* output){
No* atual = lista->inicio;
while(atual != NULL){
    if(strcmp(atual->nome, pessoa) == 0){
        if(atual->anterior != NULL) atual->anterior->proximo = atual->proximo;
        else lista->inicio = atual->proximo;
        
        if(atual->proximo != NULL) atual->proximo->anterior = atual->anterior;
        else lista->fim = atual->anterior;
    free(atual);
    char nome_temp[50];
    strcpy(nome_temp, pessoa);
    modificar_nome(nome_temp);
    fprintf(output, "[SUCCESS]REMOVE=%s\n", nome_temp);
    return;
}
atual = atual->proximo;
}
char nome_temp[50];
strcpy(nome_temp, pessoa);
modificar_nome(nome_temp);
fprintf(output, "[FAILURE]REMOVE=%s\n", nome_temp);
}

void imprimir_pessoa_amigos(Lista* lista, char* pessoa, FILE* output){
No* atual = lista->inicio;
    while(atual != NULL){
        if(strcmp(atual->nome, pessoa) == 0){
            char atual_temp[50], anterior_temp[50], proximo_temp[50];
            strcpy(atual_temp, pessoa);
            modificar_nome(atual_temp);

            if(atual->anterior != NULL) strcpy(anterior_temp, atual->anterior->nome);
            else strcpy(anterior_temp, lista->fim->nome);
            modificar_nome(anterior_temp);

            if(atual->proximo != NULL) strcpy(proximo_temp, atual->proximo->nome);
            else strcpy(proximo_temp, lista->inicio->nome);
            modificar_nome(proximo_temp);

            fprintf(output, "[SUCCESS]SHOW=%s<-%s->%s\n", anterior_temp, atual_temp, proximo_temp);
            return;
        }
    atual = atual->proximo;
    }
char nome_temp[50];
strcpy(nome_temp, pessoa);
modificar_nome(nome_temp);
fprintf(output, "[FAILURE]SHOW=?<-%s->?\n", nome_temp);
}

int main(int argc, char* argv[]){
Lista lista;
lista.inicio = NULL;
lista.fim = NULL;

FILE* input = fopen(argv[1], "r");
if(input == NULL){
    printf("Erro ao abrir o arquivo de input");
    return 1;
}

FILE* output = fopen(argv[2], "w");
if(output == NULL){
    printf("Erro ao abrir o arquivo de input");
    return 1;
}

char comando[10], nome[50];
while(fscanf(input, "%s %[^\n]", comando, nome) != EOF){ //com o %[^\n] eu consigo ler a linha toda até \n
//o fgets antes ele não consumia os espaços entre os comandos, as setas com o começo dos nomes
//ou seja ele lia o nome como " Jose da Silva", ai a função pra substituir os " " colocava o _ em tudo
    if(strcmp(comando, "ADD") == 0)inserir_final(&lista, nome, output);

    else if(strcmp(comando, "REMOVE") == 0) remover_pessoa(&lista, nome, output);

    else if(strcmp(comando, "SHOW") == 0) imprimir_pessoa_amigos(&lista, nome, output);
}

liberar_memoria(&lista);
fclose(input);
fclose(output);
return 0;
}