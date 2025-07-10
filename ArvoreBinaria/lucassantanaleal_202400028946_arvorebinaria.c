#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

//gcc -Wall lucassantanaleal_202400028946_arvorebinaria.c -o lucassantanaleal_202400028946_arvorebinaria
//./lucassantanaleal_202400028946_arvorebinaria input.txt output.txt

typedef struct Arquivo{
char nome[51], tipo[3];
uint64_t tamanho;
uint32_t indice;
} Arquivo;

typedef struct Arvore{
Arquivo* arquivo_no;
struct Arvore* Esquerda;
struct Arvore* Direita;
} Arvore;

Arvore* cria_arv_vazia () {
   return NULL;
}

Arvore* Inserir_No(Arvore* a, Arquivo arquivo_inserido){
    if(a == NULL){
        a = (Arvore*) malloc(sizeof(Arvore));
        a->arquivo_no = (Arquivo*) malloc(sizeof(Arquivo));
        if(a->arquivo_no == 0){
            printf("Erro ao alocar o no");
            exit(1);
        }
        strcpy(a->arquivo_no->nome, arquivo_inserido.nome);
        strcpy(a->arquivo_no->tipo, arquivo_inserido.tipo);
        a->arquivo_no->tamanho = arquivo_inserido.tamanho;
        a->arquivo_no->indice = arquivo_inserido.indice;

        a->Esquerda = NULL;
        a->Direita = NULL;
    }

    else if(strcmp(a->arquivo_no->nome, arquivo_inserido.nome) > 0){
        a->Esquerda = Inserir_No(a->Esquerda, arquivo_inserido);
    }

    else if(strcmp(a->arquivo_no->nome, arquivo_inserido.nome) < 0){
        a->Direita = Inserir_No(a->Direita, arquivo_inserido);
    }

    else if(strcmp(a->arquivo_no->nome, arquivo_inserido.nome) == 0 && strcmp(a->arquivo_no->tipo, "rw") == 0){
        strcpy(a->arquivo_no->tipo, arquivo_inserido.tipo);
        a->arquivo_no->tamanho = arquivo_inserido.tamanho;
        a->arquivo_no->indice = arquivo_inserido.indice;
    }

    return a;
}

void libera_arvore(Arvore* a) {
    if (a != NULL) {
        libera_arvore(a->Esquerda);
        libera_arvore(a->Direita);
        free(a->arquivo_no);
        free(a);
    }
}

void in_ordem(Arvore *arvore, FILE* output) {
   if (arvore != NULL) {
      in_ordem (arvore->Esquerda, output);
      if(arvore->arquivo_no->tamanho == 1){
        fprintf (output, "%u:%s|%s|%" PRIu64 "_byte\n", arvore->arquivo_no->indice, arvore->arquivo_no->nome, arvore->arquivo_no->tipo, arvore->arquivo_no->tamanho);
      }
      else{
        fprintf (output, "%u:%s|%s|%" PRIu64 "_bytes\n", arvore->arquivo_no->indice, arvore->arquivo_no->nome, arvore->arquivo_no->tipo, arvore->arquivo_no->tamanho);
      }
      in_ordem (arvore->Direita, output);
   }
}

void pre_ordem(Arvore *arvore, FILE* output) {
    if (arvore != NULL) {
        if(arvore->arquivo_no->tamanho == 1){
            fprintf (output, "%u:%s|%s|%" PRIu64 "_byte\n", arvore->arquivo_no->indice, arvore->arquivo_no->nome, arvore->arquivo_no->tipo, arvore->arquivo_no->tamanho);
      }
        else{
            fprintf (output, "%u:%s|%s|%" PRIu64 "_bytes\n", arvore->arquivo_no->indice, arvore->arquivo_no->nome, arvore->arquivo_no->tipo, arvore->arquivo_no->tamanho);
      }
        pre_ordem (arvore->Esquerda, output);
        pre_ordem (arvore->Direita, output);
    }
}

void pos_ordem(Arvore *arvore, FILE* output) {
   if (arvore != NULL) {
        pos_ordem (arvore->Esquerda, output);
        pos_ordem (arvore->Direita, output);
        if(arvore->arquivo_no->tamanho == 1){
            fprintf (output, "%u:%s|%s|%" PRIu64 "_byte\n", arvore->arquivo_no->indice, arvore->arquivo_no->nome, arvore->arquivo_no->tipo, arvore->arquivo_no->tamanho);
        }
        else{
            fprintf (output, "%u:%s|%s|%" PRIu64 "_bytes\n", arvore->arquivo_no->indice, arvore->arquivo_no->nome, arvore->arquivo_no->tipo, arvore->arquivo_no->tamanho);
        }
   }
}


int main(int argc, char* argv[]){
    FILE* input = fopen(argv[1], "r");;
    if(input == NULL){
        printf("Erro ao abrir arquivo de input");
        exit(1);
    }
    FILE* output = fopen(argv[2], "w");;
    if(output == NULL){
        perror("Erro ao abrir arquivo de output");
        exit(1);
    }
    int quantidade_documentos;
    fscanf(input, "%d", &quantidade_documentos);
    
    Arquivo *vetor;
    vetor = (Arquivo*) malloc(quantidade_documentos * sizeof(Arquivo));


    for(int i = 0; i < quantidade_documentos; i++){
        fscanf(input, "%50s %2s %" SCNu64 "", vetor[i].nome, vetor[i].tipo, &vetor[i].tamanho);
        //fscanf(input, "%50[^ ] %2[^ ] %" SCNu64 "", &vetor[i].nome, &vetor[i].tipo, &vetor[i].tamanho);
        //fscanf(input, "%50[^ ] %2[^ ] %[^\n]", &vetor[i].nome, &vetor[i].tipo, &vetor[i].tamanho);
    }
    
    Arvore *arvore_busca = cria_arv_vazia();

    for(int i = 0; i < quantidade_documentos; i++){
        vetor[i].indice = i;
        arvore_busca = Inserir_No(arvore_busca, vetor[i]);
    }

    fprintf(output, "[EPD]\n");
    in_ordem(arvore_busca, output);
    fprintf(output, "[PED]\n");
    pre_ordem(arvore_busca, output);
    fprintf(output, "[EDP]\n");
    pos_ordem(arvore_busca, output);
    
    fclose(input);
    fclose(output);
    free(vetor);
    libera_arvore(arvore_busca);
    return 0;
}