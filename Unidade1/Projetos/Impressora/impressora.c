#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

//gcc impressora.c -o impressora
//./impressora input.txt output.txt

typedef struct elemento{
    struct elemento* proximo;
    uint32_t paginas;
    char nome[51];
} elemento;

typedef struct fila{
elemento* inicio;
elemento* fim;
} fila;

typedef struct pilha{
    elemento* topo;
} pilha;

typedef struct impressora{
    pilha* pilha_pessoal;
    char nome[100];
    uint32_t tempo_ocupado_ate;
} impressora;

typedef struct evento { //evento é o documento após ser impresso
    char nome_doc[51];
    uint32_t paginas;
    uint32_t tempo_final;
} evento;

int comparar_eventos(const void* a, const void* b) {
    evento* ea = (evento*)a;
    evento* eb = (evento*)b;
    return (ea->tempo_final - eb->tempo_final);
}

void trocar(evento* a, evento* b) {
    evento temp = *a;
    *a = *b;
    *b = temp;
}

int particionar(evento* arr, int baixo, int alto) {
    uint32_t pivo = arr[alto].tempo_final;
    int i = baixo - 1;

    for (int j = baixo; j < alto; j++) {
        if (arr[j].tempo_final < pivo) {
            i++;
            trocar(&arr[i], &arr[j]);
        }
    }

    trocar(&arr[i + 1], &arr[alto]);
    return i + 1;
}

void quicksort(evento* arr, int baixo, int alto) {
    if (baixo < alto) {
        int pi = particionar(arr, baixo, alto);
        quicksort(arr, baixo, pi - 1);
        quicksort(arr, pi + 1, alto);
    }
}

void inserir_final_fila(fila* fila, uint32_t paginas, char* nome_documento){
elemento* novo = (elemento*) malloc(sizeof(elemento));
if(novo == NULL){
    printf("Erro ao inserir um elemento novo no final da fila\n");
    exit(1);
}

novo->proximo = NULL;
novo->paginas = paginas;
if (strlen(nome_documento) == 0 || strlen(nome_documento) > 51) {
    printf("Nome de documento invalido!\n");
    free(novo);
    return;
}

strcpy(novo->nome, nome_documento);

if(fila->fim == NULL && fila->inicio == NULL){
    fila->fim = novo;
    fila->inicio = novo;
}
else {
    fila->fim->proximo = novo;
    fila->fim = novo;
}
}

void retirar_documento_fila(fila* fila){
if(fila->inicio != NULL){
elemento* primeiro = fila->inicio;
fila->inicio = primeiro->proximo; 
if(fila->inicio == NULL){
    fila->fim = NULL;
}
free(primeiro);
}
else printf("Nao tem como retirar elemento duma fila vazia\n");
}

void liberar_fila(fila* fila){
elemento* atual = fila->inicio;
while(atual != NULL){
    elemento* temp = atual;
    atual = atual->proximo;
    free(temp);
}
fila->inicio = NULL;
fila->fim = NULL;
}

//-------------------------------------------------------------------------------------------------------------//

void inserir_topo_pilha(pilha* pilha, uint32_t paginas, char* nome_documento){
elemento* novo = (elemento*) malloc(sizeof(elemento));
if(novo == NULL){
    printf("Erro ao adicionar elemento no topo da pilha\n");
    exit(1);
}

novo->paginas = paginas;
novo->proximo = pilha->topo;
if (strlen(nome_documento) == 0 || strlen(nome_documento) > 51) {
    printf("Nome de documento invalido!\n");
    free(novo);
    return;
}

strcpy(novo->nome, nome_documento);
pilha->topo = novo;
}

void retirar_documento_pilha(pilha* pilha){
    if(pilha->topo != NULL){
        elemento* ultimo = pilha->topo;
        pilha->topo = ultimo->proximo;
        free(ultimo);
    }
    else printf("Nao tem como retirar elemento duma pilha vazia\n");
}

void liberar_pilha(pilha* pilha){
elemento* atual = pilha->topo;
while(atual != NULL){
    elemento* temp = atual;
    atual = atual->proximo;
    free(temp);
}
pilha->topo = NULL;
}

void amostrar_elementos_pilha_documentos(pilha* pilha, FILE* output){
elemento* atual = pilha->topo;
if (atual == NULL) {
    fprintf(output, "Pilha vazia.\n");
    return;
}

while(atual != NULL){
    fprintf(output, "%s-%dp\n", atual->nome, atual->paginas);
    atual = atual->proximo;
}
}

//-------------------------------------------------------------------------------------------------------------//

int encontrar_impressora_mais_disponivel(impressora* impressoras, int qtd_impressoras) {
    int indice_menor = 0;
    for (int i = 0; i < qtd_impressoras; i++) {
        if (impressoras[i].tempo_ocupado_ate < impressoras[indice_menor].tempo_ocupado_ate) {
            indice_menor = i;
        }
    }
    return indice_menor;
}

void amostrar_pilha_impressora(impressora* impressora, FILE* output) {
elemento* atual = impressora->pilha_pessoal->topo;

fprintf(output, "%s:", impressora->nome);

if (atual == NULL) {
    fprintf(output, "[vazio]\n");
    return;
}

while (atual != NULL) {
    fprintf(output, "%s-%up", atual->nome, atual->paginas);
    if (atual->proximo != NULL) fprintf(output, ",");
    atual = atual->proximo;
}
fprintf(output, "\n");
}

void processar_documentos(impressora* impressoras, int qtd_impressoras, fila* fila_documentos, pilha* pilha_final, FILE* output, int quantidade_documentos) {
    uint32_t total_paginas = 0;
    int qtd_eventos = 0;
    evento* eventos = malloc(sizeof(evento) * quantidade_documentos);

    uint32_t tempo_global = 0;

    while (fila_documentos->inicio != NULL) {
        int idx = encontrar_impressora_mais_disponivel(impressoras, qtd_impressoras);
        elemento* doc = fila_documentos->inicio;

    // Tempo de início é o maior entre o tempo atual e o tempo que a impressora ficará livre
        if (tempo_global < impressoras[idx].tempo_ocupado_ate) {
            tempo_global = impressoras[idx].tempo_ocupado_ate;
        }

        uint32_t inicio_impressao = tempo_global;
        uint32_t fim_impressao = inicio_impressao + doc->paginas;

        impressoras[idx].tempo_ocupado_ate = fim_impressao;
        tempo_global = inicio_impressao; // mantém tempo atual para a próxima rodada

        inserir_topo_pilha(impressoras[idx].pilha_pessoal, doc->paginas, doc->nome);

        strcpy(eventos[qtd_eventos].nome_doc, doc->nome);
        eventos[qtd_eventos].paginas = doc->paginas;
        eventos[qtd_eventos].tempo_final = fim_impressao;
        qtd_eventos++;

        total_paginas += doc->paginas;

        amostrar_pilha_impressora(&impressoras[idx], output);
        retirar_documento_fila(fila_documentos);
}

    // Ordena eventos pela ordem de término
    quicksort(eventos, 0, qtd_eventos - 1);

    // Monta pilha final na ordem correta
    for (int i = 0; i < qtd_eventos; i++) {
        inserir_topo_pilha(pilha_final, eventos[i].paginas, eventos[i].nome_doc);
    }

    fprintf(output, "%up\n", total_paginas);

    free(eventos);
}

int main(int argc, char* argv[]){
fila documentos_a_serem_impressos;
documentos_a_serem_impressos.fim = NULL;
documentos_a_serem_impressos.inicio = NULL;

pilha pilha_final;
pilha_final.topo = NULL;

FILE* input = fopen(argv[1], "r");
if(input == NULL){
    printf("Erro ao abrir o arquivo de input");
    return 1;
}
FILE* output = fopen(argv[2], "w");
if(output == NULL){
    printf("Erro ao abrir o arquivo de output");
    return 1;
}

int quantidade_impressoras, quantidade_documentos;
fscanf(input, "%d", &quantidade_impressoras);

impressora* impressoras = malloc(sizeof(impressora) * quantidade_impressoras);
for (int i = 0; i < quantidade_impressoras; i++) {
    impressoras[i].pilha_pessoal = malloc(sizeof(pilha));
    impressoras[i].pilha_pessoal->topo = NULL;
    impressoras[i].tempo_ocupado_ate = 0;
    fscanf(input, "%s", impressoras[i].nome);
}

fscanf(input, "%d", &quantidade_documentos);

for (int i = 0; i < quantidade_documentos; i++) {
    char nome_doc[51];
    int paginas;
    fscanf(input, "%s %d", nome_doc, &paginas);
    inserir_final_fila(&documentos_a_serem_impressos, paginas, nome_doc);
}

processar_documentos(impressoras, quantidade_impressoras, &documentos_a_serem_impressos, &pilha_final, output, quantidade_documentos);
amostrar_elementos_pilha_documentos(&pilha_final, output);


liberar_fila(&documentos_a_serem_impressos);
liberar_pilha(&pilha_final);
for (int i = 0; i < quantidade_impressoras; i++) {
liberar_pilha(impressoras[i].pilha_pessoal);
free(impressoras[i].pilha_pessoal);
}
free(impressoras);

fclose(input);
fclose(output);
return 0;
}