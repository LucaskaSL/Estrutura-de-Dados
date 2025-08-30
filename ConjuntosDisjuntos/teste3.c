#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

// gcc -Wall teste.c -o teste
// ./teste input.txt output.txt

typedef struct no {
    uint32_t altura;
    struct no* pai;
    uint32_t coordenada_x;
    uint32_t coordenada_y;
} no;

no* criar_no() {
    no* no_novo = (no*)malloc(sizeof(no));
    no_novo->altura = 0;
    no_novo->coordenada_x = 0;
    no_novo->coordenada_y = 0;
    no_novo->pai = no_novo;
    return no_novo;
}

no* encontrar_conjunto(no* x) {
    if(x != x->pai){
        x->pai = encontrar_conjunto(x->pai);
    }
    return x->pai;
}

void unir_conjuntos(no* x, no* y){
    no* rx = encontrar_conjunto(x);
    no* ry = encontrar_conjunto(y);

    if(rx->altura > ry->altura){
        ry->pai = rx;
    }
    else{
        rx->pai = ry;
        if(rx->altura == ry->altura){
            ry->altura++;
       }
    }
}

void liberar_no(no* no_ptr) {
    free(no_ptr);
}

uint32_t myrand() {
    static uint32_t next = 1;
    static uint32_t call_count = 0;
    next = next * 1103515245 + 12345;
    call_count++;
    printf("myrand call #%u: returning %u\n", call_count, next);
    return next;
}

int32_t geradora(int32_t z){
    uint32_t rand_val = myrand() % 3;
    int32_t result = z + (-1 + rand_val);
    printf("geradora(%d): myrand()%%3 = %u, result = %d\n", z, rand_val, result);
    return result;
}

void propagar_cadeia(no*** matriz, no* paciente_zero, no* infectante_atual, uint32_t altura, uint32_t largura, FILE* output, uint32_t infectados_count, uint32_t total_pessoas) {
    if (infectados_count >= total_pessoas) {
        return;
    }
    
    printf("\n--- Propagação #%u a partir de (%u,%u) ---\n", 
           infectados_count, infectante_atual->coordenada_x, infectante_atual->coordenada_y);
    
    int32_t novo_x, novo_y;
    no* proximo_alvo = NULL;
    
    // Primeiro: calcular g(x)
    printf("Calculando g(x):\n");
    while (true) {
        novo_x = geradora(infectante_atual->coordenada_x);
        printf("novo_x = %d, limites: [0, %u)\n", novo_x, largura);
        if (novo_x >= 0 && novo_x < (int32_t)largura) {
            break;
        }
        printf("x fora dos limites, recalculando...\n");
    }
    
    // Depois: calcular g(y)  
    printf("Calculando g(y):\n");
    while (true) {
        novo_y = geradora(infectante_atual->coordenada_y);
        printf("novo_y = %d, limites: [0, %u)\n", novo_y, altura);
        if (novo_y >= 0 && novo_y < (int32_t)altura) {
            break;
        }
        printf("y fora dos limites, recalculando...\n");
    }
    
    proximo_alvo = matriz[novo_x][novo_y];
    
    printf("Alvo: (%d,%d), conjunto: %p\n", novo_x, novo_y, (void*)encontrar_conjunto(proximo_alvo));
    printf("Paciente zero conjunto: %p\n", (void*)encontrar_conjunto(paciente_zero));
    
    // Verificar se já está infectado
    if (encontrar_conjunto(proximo_alvo) == encontrar_conjunto(paciente_zero)) {
        printf("ALVO JÁ INFECTADO! Recursando...\n");
        propagar_cadeia(matriz, paciente_zero, infectante_atual, altura, largura, output, infectados_count, total_pessoas);
        return;
    }
    
    // Infectar o novo alvo
    unir_conjuntos(paciente_zero, proximo_alvo);
    fprintf(output, ";(%d,%d)", novo_x, novo_y);
    printf("INFECTADO: (%d,%d)\n", novo_x, novo_y);
    
    // Continuar a cadeia
    propagar_cadeia(matriz, paciente_zero, proximo_alvo, altura, largura, output, infectados_count + 1, total_pessoas);
}

int main(int argc, char* argv[]) {
    if (argc < 3) {
        printf("Uso: %s input.txt output.txt\n", argv[0]);
        return 1;
    }
    
    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Erro ao abrir o arquivo de input");
        exit(1);
    }
    FILE* output = fopen(argv[2], "w");
    if (output == NULL) {
        perror("Erro ao abrir o arquivo de output");
        exit(1);
    }

    uint32_t quantidade_regioes;
    fscanf(input, "%u", &quantidade_regioes);

    for (int i = 0; i < quantidade_regioes; i++) {
        uint32_t altura, largura, epicentro_x, epicentro_y;
        fscanf(input, "%u %u %u %u", &altura, &largura, &epicentro_x, &epicentro_y);

        printf("========================================\n");
        printf("REGIÃO %d: %ux%u, epicentro (%u,%u)\n", 
               i+1, altura, largura, epicentro_x, epicentro_y);
        printf("========================================\n");

        no*** matriz = (no***)malloc(sizeof(no**) * largura);
        for (int x = 0; x < largura; x++) {
            matriz[x] = (no**)malloc(sizeof(no*) * altura);
        }

        for (int x = 0; x < largura; x++) {
            for (int y = 0; y < altura; y++) {
                matriz[x][y] = criar_no();
                matriz[x][y]->coordenada_x = x;
                matriz[x][y]->coordenada_y = y;
            }
        }

        fprintf(output, "%d:(%u,%u)", i + 1, epicentro_x, epicentro_y);
        printf("Epicentro: (%u,%u)\n", epicentro_x, epicentro_y);

        uint32_t total_pessoas = altura * largura;
        propagar_cadeia(matriz, matriz[epicentro_x][epicentro_y], 
                       matriz[epicentro_x][epicentro_y], altura, largura, 
                       output, 1, total_pessoas);

        fprintf(output, "\n");

        for (int x = 0; x < largura; x++) {
            for (int y = 0; y < altura; y++) {
                liberar_no(matriz[x][y]);
            }
            free(matriz[x]);
        }
        free(matriz);
        
        printf("========================================\n");
    }

    fclose(input);
    fclose(output);
    return 0;
}