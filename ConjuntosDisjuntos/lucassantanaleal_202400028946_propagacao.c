#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//gcc -Wall teste.c -o teste
//./teste input.txt output.txt

typedef struct no{
    uint32_t altura;
    struct no* pai;
    uint32_t coordenada_x;
    uint32_t coordenada_y;
    bool infectado;
} no;

no* criar_no(){
    no* no_novo = (no*) malloc(sizeof(no));
    no_novo->altura = 0;
    no_novo->coordenada_x = 0;
    no_novo->coordenada_y = 0;
    no_novo->infectado = false;
    no_novo->pai = no_novo;
    return no_novo;
}

void liberar_no(no* no_ptr){
    free(no_ptr);
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

uint32_t myrand(){
    static uint32_t next = 1;
    next = next * 1103515245 + 12345;
    return next;
}

int32_t geradora(int32_t z){
    return z + (-1 + (myrand() % 3));
}

bool tentar_infectar(no*** matriz, no* infectante, uint32_t altura, uint32_t largura, FILE* output) {
    int32_t novo_x = geradora((int32_t)infectante->coordenada_x);
    int32_t novo_y = geradora((int32_t)infectante->coordenada_y);
    
    if(novo_x >= 0 && novo_x < (int32_t)largura && novo_y >= 0 && novo_y < (int32_t)altura) {
        no* alvo = matriz[novo_x][novo_y];
        
        if(!alvo->infectado) {
            alvo->infectado = true;
            fprintf(output, ";(%d,%d)", novo_x, novo_y);
            unir_conjuntos(infectante, alvo);
            return true; 
        }
    }
    return false;
}

int main(int argc, char* argv[]){
    FILE* input = fopen(argv[1], "r");
    if(input == NULL){
        perror("Erro ao abrir o arquivo de input");
        exit(1);
    }
    FILE* output = fopen(argv[2], "w");
    if(output == NULL){
        perror("Erro ao abrir o arquivo de output");
        exit(1);
    }

    uint32_t quantidade_regioes;
    fscanf(input, "%u", &quantidade_regioes);

    for(int i = 0; i < quantidade_regioes; i++){
        uint32_t altura, largura, epicentro_x, epicentro_y;
        fscanf(input, "%u %u %u %u", &altura, &largura, &epicentro_x, &epicentro_y);

        no*** matriz = (no***) malloc(sizeof(no**) * largura);
        for(int x = 0; x < largura; x++){
            matriz[x] = (no**) malloc(sizeof(no*) * altura);
        }

        for(int x = 0; x < largura; x++){
            for(int y = 0; y < altura; y++){
                matriz[x][y] = criar_no();
                matriz[x][y]->coordenada_x = x;
                matriz[x][y]->coordenada_y = y;
            }
        }

        matriz[epicentro_x][epicentro_y]->infectado = true;
        fprintf(output, "%d:(%u,%u)", i + 1, epicentro_x, epicentro_y);

        bool todos_infectados = false;
        uint32_t iteracao = 0;
        const uint32_t max_iteracoes = 10000000; // Prevenção contra loop infinito

        while(!todos_infectados && iteracao < max_iteracoes) {
            iteracao++;
            todos_infectados = true;

            // Verificar se todos estão infectados
            for(int x = 0; x < largura; x++) {
                for(int y = 0; y < altura; y++) {
                    if(!matriz[x][y]->infectado) {
                        todos_infectados = false;
                    }
                }
            }

            // Se não estão todos infectados, tentar infectar
            if(!todos_infectados) {
                for(int x = 0; x < largura; x++) {
                    for(int y = 0; y < altura; y++) {
                        if(matriz[x][y]->infectado) {
                            // Cada infectado tenta infectar várias vezes
                            for(int tentativa = 0; tentativa < 10; tentativa++) {
                                if(tentar_infectar(matriz, matriz[x][y], altura, largura, output)) {
                                    break;
                                }
                            }
                        }
                    }
                }
            }
        }

        fprintf(output, "\n");

        for(int x = 0; x < largura; x++){
            for(int y = 0; y < altura; y++){
                liberar_no(matriz[x][y]);
            }
            free(matriz[x]);
        }
        free(matriz);
    }

    fclose(input);
    fclose(output);
    return 0;
}