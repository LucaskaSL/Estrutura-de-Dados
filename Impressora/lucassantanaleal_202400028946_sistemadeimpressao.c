#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>    // Para srand() e time()
#include <stdbool.h> // Para usar bool

// Para compilar com otimizações:
// gcc impressora.c -o impressora -O3
// Para executar:
// ./impressora input.txt output.txt

typedef struct elemento{
    struct elemento* proximo;
    uint32_t paginas;
    char nome[51]; // Suporta até 50 caracteres + \0
    uint32_t original_queue_index; // Índice original na fila de entrada
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
    char nome[100]; // Suporta até 99 caracteres + \0
    uint32_t tempo_ocupado_ate; // Tempo em que esta impressora ficará livre
} impressora;

// MODIFICADO: Adicionado 'indice_impressora'
typedef struct evento { //evento é o documento após ser impresso
    char nome_doc[51]; // Suporta até 50 caracteres + \0
    uint32_t paginas;
    uint32_t tempo_final; // Tempo de conclusão da impressão
    uint32_t original_queue_index; // Índice original na fila de entrada para desempate
    uint32_t indice_impressora; // NOVO CAMPO: Índice da impressora que imprimiu
} evento;

void trocar_evento(evento* a, evento* b) {
    evento temp = *a;
    *a = *b;
    *b = temp;
}

// MODIFICADO: Nova lógica de comparação
int comparar_eventos(const void* a, const void* b) {
    evento* ea = (evento*)a;
    evento* eb = (evento*)b;

    // Comparação primária pelo tempo_final (ASCENDENTE)
    if (ea->tempo_final < eb->tempo_final) {
        return -1; // ea vem antes de eb
    } else if (ea->tempo_final > eb->tempo_final) {
        return 1; // ea vem depois de eb
    } else {
        // Se os tempos finais são iguais, desempate pelo índice da impressora (ASCENDENTE)
        // A impressora de menor índice tem prioridade.
        if (ea->indice_impressora < eb->indice_impressora) {
            return -1; // ea (impressora de menor índice) vem antes
        } else if (ea->indice_impressora > eb->indice_impressora) {
            return 1; // eb (impressora de menor índice) vem antes
        } else {
            // Se o tempo e a impressora forem os mesmos (não deve acontecer na prática),
            // usa a ordem original da fila como último critério.
            return (int)(ea->original_queue_index - eb->original_queue_index);
        }
    }
}

int particionar_evento(evento* arr, int baixo, int alto) {
    int random_pivot_index = baixo + rand() % (alto - baixo + 1);
    trocar_evento(&arr[random_pivot_index], &arr[alto]);
    evento pivo = arr[alto];
    int i = baixo - 1;
    for (int j = baixo; j < alto; j++) {
        if (comparar_eventos(&arr[j], &pivo) <= 0) {
            i++;
            trocar_evento(&arr[i], &arr[j]);
        }
    }
    trocar_evento(&arr[i + 1], &arr[alto]);
    return i + 1;
}

void quicksort_eventos(evento* arr, int baixo, int alto) {
    if (baixo < alto) {
        int pi = particionar_evento(arr, baixo, alto);
        quicksort_eventos(arr, baixo, pi - 1);
        quicksort_eventos(arr, pi + 1, alto);
    }
}

void inserir_final_fila(fila* fila, uint32_t paginas, char* nome_documento, uint32_t original_index){
    elemento* novo = (elemento*) malloc(sizeof(elemento));
    if(novo == NULL){ exit(1); }
    novo->proximo = NULL;
    novo->paginas = paginas;
    strcpy(novo->nome, nome_documento);
    novo->original_queue_index = original_index;
    if(fila->fim == NULL && fila->inicio == NULL){
        fila->fim = novo;
        fila->inicio = novo;
    } else {
        fila->fim->proximo = novo;
        fila->fim = novo;
    }
}

elemento* retirar_documento_fila(fila* fila){
    if(fila->inicio != NULL){
        elemento* primeiro = fila->inicio;
        fila->inicio = primeiro->proximo;
        if(fila->inicio == NULL){ fila->fim = NULL; }
        return primeiro;
    }
    return NULL;
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

void inserir_topo_pilha(pilha* pilha, uint32_t paginas, char* nome_documento){
    elemento* novo = (elemento*) malloc(sizeof(elemento));
    if(novo == NULL){ exit(1); }
    novo->paginas = paginas;
    novo->proximo = pilha->topo;
    strcpy(novo->nome, nome_documento);
    pilha->topo = novo;
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

int encontrar_impressora_mais_disponivel(impressora* impressoras, int qtd_impressoras) {
    int indice_menor = 0;
    for (int i = 0; i < qtd_impressoras; i++) {
        if (impressoras[i].tempo_ocupado_ate < impressoras[indice_menor].tempo_ocupado_ate) {
            indice_menor = i;
        } else if (impressoras[i].tempo_ocupado_ate == impressoras[indice_menor].tempo_ocupado_ate) {
            if (i < indice_menor) {
                indice_menor = i;
            }
        }
    }
    return indice_menor;
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

void processar_documentos(impressora* impressoras, int qtd_impressoras, fila* fila_documentos, pilha* pilha_final, FILE* output, int quantidade_documentos) {
    uint32_t total_paginas = 0;
    int qtd_eventos = 0;
    evento* eventos = malloc(sizeof(evento) * quantidade_documentos);
    if (eventos == NULL) { exit(1); }

    while (fila_documentos->inicio != NULL) {
        int idx_impressora = encontrar_impressora_mais_disponivel(impressoras, qtd_impressoras);
        elemento* doc = retirar_documento_fila(fila_documentos);
        if (doc == NULL) break;

        uint32_t inicio_impressao = impressoras[idx_impressora].tempo_ocupado_ate;
        uint32_t fim_impressao = inicio_impressao + doc->paginas;
        impressoras[idx_impressora].tempo_ocupado_ate = fim_impressao;
        inserir_topo_pilha(impressoras[idx_impressora].pilha_pessoal, doc->paginas, doc->nome);

        strcpy(eventos[qtd_eventos].nome_doc, doc->nome);
        eventos[qtd_eventos].paginas = doc->paginas;
        eventos[qtd_eventos].tempo_final = fim_impressao;
        eventos[qtd_eventos].original_queue_index = doc->original_queue_index;
        eventos[qtd_eventos].indice_impressora = idx_impressora; // MODIFICADO: Salva o índice
        qtd_eventos++;
        total_paginas += doc->paginas;

        amostrar_pilha_impressora(&impressoras[idx_impressora], output);
        free(doc);
    }

    quicksort_eventos(eventos, 0, qtd_eventos - 1);

    for (int i = 0; i < qtd_eventos; i++) {
        inserir_topo_pilha(pilha_final, eventos[i].paginas, eventos[i].nome_doc);
    }

    fprintf(output, "%up\n", total_paginas);
    free(eventos);
}

int main(int argc, char* argv[]){
    srand(time(NULL));

    fila documentos_a_serem_impressos;
    documentos_a_serem_impressos.fim = NULL;
    documentos_a_serem_impressos.inicio = NULL;

    pilha pilha_final;
    pilha_final.topo = NULL;

    if (argc != 3) { return 1; }
    FILE* input = fopen(argv[1], "r");
    if(input == NULL){ return 1; }
    FILE* output = fopen(argv[2], "w");
    if(output == NULL){ fclose(input); return 1; }

    int quantidade_impressoras;
    fscanf(input, "%d", &quantidade_impressoras);

    impressora* impressoras = malloc(sizeof(impressora) * quantidade_impressoras);
    if (impressoras == NULL) { return 1; }

    for (int i = 0; i < quantidade_impressoras; i++) {
        impressoras[i].pilha_pessoal = malloc(sizeof(pilha));
        if (impressoras[i].pilha_pessoal == NULL) { return 1; }
        impressoras[i].pilha_pessoal->topo = NULL;
        impressoras[i].tempo_ocupado_ate = 0;
        fscanf(input, "%99s", impressoras[i].nome);
    }

    int quantidade_documentos;
    fscanf(input, "%d", &quantidade_documentos);

    for (int i = 0; i < quantidade_documentos; i++) {
        char nome_doc[51];
        int paginas;
        fscanf(input, "%50s %d", nome_doc, &paginas);
        inserir_final_fila(&documentos_a_serem_impressos, paginas, nome_doc, i);
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