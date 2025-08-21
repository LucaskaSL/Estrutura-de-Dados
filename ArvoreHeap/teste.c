#include <stdlib.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

//gcc teste.c -o teste
//./teste input.txt output.txt

typedef struct apostadores{
    char chave[33];
    uint32_t aposta[15];
    uint32_t quantidade_acertos;
} apostadores;

typedef struct heap{
    apostadores* apostadores;
    uint32_t tamanho;
    uint32_t capacidade;
} Heap;

void trocar_apostadores(apostadores* a, apostadores* b)
{
    apostadores temp = *a;
    *a = *b;
    *b = temp;
}

void heapify_max(Heap* heap, uint32_t i) {
    uint32_t Pai = i, Esquerda = 2*i + 1, Direita = 2*i + 2;

    if(Esquerda < heap->tamanho && heap->apostadores[Esquerda].quantidade_acertos > heap->apostadores[Pai].quantidade_acertos)
        Pai = Esquerda;
    if(Direita < heap->tamanho && heap->apostadores[Direita].quantidade_acertos > heap->apostadores[Pai].quantidade_acertos)
        Pai = Direita;

    if(Pai != i) {
    trocar_apostadores(&heap->apostadores[i], &heap->apostadores[Pai]);
    heapify_max(heap, Pai);
    }
}

void heapify_min(Heap* heap, uint32_t i) {
    uint32_t Pai = i, Esquerda = 2*i + 1, Direita = 2*i + 2;

    if(Esquerda < heap->tamanho && heap->apostadores[Esquerda].quantidade_acertos < heap->apostadores[Pai].quantidade_acertos)
        Pai = Esquerda;
    if(Direita < heap->tamanho && heap->apostadores[Direita].quantidade_acertos < heap->apostadores[Pai].quantidade_acertos)
        Pai = Direita;

    if(Pai != i) {
    trocar_apostadores(&heap->apostadores[i], &heap->apostadores[Pai]);
    heapify_min(heap, Pai);
    }
}

Heap* criar_heap(uint32_t capacidade){
    Heap* heap = (Heap*) malloc(sizeof(Heap));
    heap->capacidade = capacidade;
    heap->tamanho = 0;
    heap->apostadores = (apostadores*) malloc(sizeof(apostadores) * heap->capacidade);
    return heap;
}

void inserir_heap(Heap* heap, apostadores apostador_inserido){
    if (heap->tamanho == heap->capacidade) {
        return;
    }

    heap->tamanho++;
    int i = heap->tamanho - 1;
    heap->apostadores[i] = apostador_inserido;

    while (i != 0 && heap->apostadores[(i - 1) / 2].quantidade_acertos < heap->apostadores[i].quantidade_acertos) {
        trocar_apostadores(&heap->apostadores[i], &heap->apostadores[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

apostadores extrair_Max(Heap* heap){
     if (heap->tamanho <= 0) {
        apostadores vazio = {0};
        return vazio;
    }

    if (heap->tamanho == 1) {
        heap->tamanho--;
        return heap->apostadores[0];
    }

    //Guarda o valor máximo e tira ele da heap
    apostadores raiz = heap->apostadores[0];
    heap->apostadores[0] = heap->apostadores[heap->tamanho - 1];
    heap->tamanho--;
    heapify_max(heap, 0);

    return raiz;
}

apostadores extrair_Min(Heap* heap){
     if (heap->tamanho <= 0) {
        apostadores vazio = {0};
        return vazio;
    }

    if (heap->tamanho == 1) {
        heap->tamanho--;
        return heap->apostadores[0];
    }

    //Guarda o valor mínimo e tira ele da heap
    apostadores raiz = heap->apostadores[0];
    heap->apostadores[0] = heap->apostadores[heap->tamanho - 1];
    heap->tamanho--;
    heapify_min(heap, 0);

    return raiz;
}

int main(int argc, char* argv[]){
    FILE* input = fopen(argv[1], "r");
    if(input == NULL){
        perror("Nao foi possivel abrir o arquivo de input");
        exit(1);
    }
    FILE* output = fopen(argv[2], "w");
    if(output == NULL){
        perror("Nao foi possivel abrir o arquivo de output");
        exit(1);
    }

    uint32_t valor_premio = 0, quantidade_apostas = 0;
    fscanf(input, "%u", &valor_premio);
    fscanf(input, "%u", &quantidade_apostas);

    uint32_t metade_maior = valor_premio/2;
    uint32_t metade_menor = valor_premio/2;

    uint32_t premio[10];
    fscanf(input, "%u %u %u %u %u %u %u %u %u %u", &premio[0], &premio[1], &premio[2], &premio[3], &premio[4], &premio[5], &premio[6], &premio[7], &premio[8], &premio[9]);

    apostadores* apostador = (apostadores*) malloc(sizeof(apostadores) * quantidade_apostas);
    for(int i = 0; i < quantidade_apostas; i++){
        fscanf(input, "%s %u %u %u %u %u %u %u %u %u %u %u %u %u %u %u", apostador[i].chave, &apostador[i].aposta[0], &apostador[i].aposta[1], &apostador[i].aposta[2], 
        &apostador[i].aposta[3], &apostador[i].aposta[4], &apostador[i].aposta[5], &apostador[i].aposta[6], &apostador[i].aposta[7], &apostador[i].aposta[8], 
        &apostador[i].aposta[9], &apostador[i].aposta[10], &apostador[i].aposta[11], &apostador[i].aposta[12], &apostador[i].aposta[13], &apostador[i].aposta[14]);
    }

    
    for(int i = 0; i < quantidade_apostas; i++){
        for(int j = 0; j < 15; j++){
            for(int k = 0; k < 10; k++){
                if(premio[k] == apostador[i].aposta[j]){
                    apostador[i].quantidade_acertos++;
                }
            }
        }
    }
    

    /*
    bool numeros_sorteados[51] = {false};
    for (int k = 0; k < 10; k++) {
        if (premio[k] <= 50) {
            numeros_sorteados[premio[k]] = true;
        }
    }

    for(int i = 0; i < quantidade_apostas; i++){
        for(int j = 0; j < 15; j++){
            uint32_t num_apostado = apostador[i].aposta[j];
            if (num_apostado <= 50 && numeros_sorteados[num_apostado]) {
                apostador[i].quantidade_acertos++;
            }
        }
    }
    */

    Heap* heap = criar_heap(quantidade_apostas);
    for(int i = 0; i < quantidade_apostas; i++){
        inserir_heap(heap, apostador[i]);
    }


    apostadores* apostador_ganhador_max = (apostadores*) malloc(sizeof(apostadores) * quantidade_apostas);
    uint32_t quantidade_ganho_maximo = 0;
    apostadores referencia_maxima = extrair_Max(heap);
    apostador_ganhador_max[0] = referencia_maxima;
    quantidade_ganho_maximo++;

    while(heap->tamanho > 0 && heap->apostadores[0].quantidade_acertos == referencia_maxima.quantidade_acertos) {
            apostador_ganhador_max[quantidade_ganho_maximo++] = extrair_Max(heap);
        }

    fprintf(output, "[%u:%u:%u]\n", quantidade_ganho_maximo, referencia_maxima.quantidade_acertos, metade_maior/quantidade_ganho_maximo);
    for(int i = 0; i < quantidade_ganho_maximo; i++){
        fprintf(output, "%s\n", apostador_ganhador_max[i].chave);
    }

    heapify_min(heap, 0);

    apostadores* apostador_ganhador_min = (apostadores*) malloc(sizeof(apostadores) * quantidade_apostas);
    uint32_t quantidade_ganho_minimo = 0;
    apostadores referencia_minima = extrair_Min(heap);
    apostador_ganhador_min[0] = referencia_minima;
    quantidade_ganho_minimo++;

    while(heap->tamanho > 0 && heap->apostadores[0].quantidade_acertos == referencia_minima.quantidade_acertos) {
            apostador_ganhador_min[quantidade_ganho_minimo++] = extrair_Min(heap);
        }

    fprintf(output, "[%u:%u:%u]\n", quantidade_ganho_minimo, referencia_minima.quantidade_acertos, metade_menor/quantidade_ganho_minimo);
    for(int i = 0; i < quantidade_ganho_minimo; i++){
        fprintf(output, "%s\n", apostador_ganhador_min[i].chave);
    }

    free(apostador_ganhador_min);
    free(apostador_ganhador_max);
    free(apostador);
    free(heap->apostadores);
    free(heap);
    fclose(input);
    fclose(output);
    return 0;
}