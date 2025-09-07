#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct no {
    struct no** ponteiros;
    uint32_t valor;
    int tem_valor;
} no;

typedef struct {
    uint32_t* indices;
    uint32_t count;
    uint32_t capacity;
} Resultados;

no* criar_no() {
    no* criado = (no*)malloc(sizeof(no));

    criado->ponteiros = (no**)calloc(26, sizeof(no*));

    criado->tem_valor = 0;
    return criado;
}

uint32_t indice(const char* palavra, uint32_t profundidade) {
    return (uint32_t)(palavra[profundidade] - 'a');
}

void insercao(no** base, const char* palavra_inserida, uint32_t tamanho_palavra, uint32_t profundidade, uint32_t novo_valor) {
    if (*base == NULL) *base = criar_no();

    if (profundidade == tamanho_palavra) {
        (*base)->valor = novo_valor;
        (*base)->tem_valor = 1;
        return;
    } else {
        insercao(&(*base)->ponteiros[indice(palavra_inserida, profundidade)], palavra_inserida, tamanho_palavra, profundidade + 1, novo_valor);
    }
}

void inicializar_resultados(Resultados* res) {
    res->count = 0;
    res->capacity = 20;
    res->indices = malloc(sizeof(uint32_t) * res->capacity);
}

void adicionar_indice(Resultados* res, uint32_t idx) {
    if (res->count >= res->capacity) {
        res->capacity *= 2;
        res->indices = realloc(res->indices, sizeof(uint32_t) * res->capacity);
    }
    res->indices[res->count++] = idx;
}

no* encontrar_no_prefixo(no* base, const char* prefixo) {
    no* atual = base;
    for (int i = 0; prefixo[i] != '\0'; i++) {
        if (atual == NULL) return NULL;
        uint32_t idx = prefixo[i] - 'a';
        atual = atual->ponteiros[idx];
    }
    return atual;
}

void coletar_sugestoes(no* no_atual, char* buffer_palavra, uint32_t comprimento_atual, Resultados* res, uint32_t max_comprimento, char* visited) {
    if (no_atual == NULL || comprimento_atual > max_comprimento) {
        return;
    }
    if (no_atual->tem_valor && !visited[no_atual->valor]) {
        adicionar_indice(res, no_atual->valor);
        visited[no_atual->valor] = 1;
    }

    for (int i = 0; i < 26; i++) {
        if (no_atual->ponteiros[i] != NULL) {
            buffer_palavra[comprimento_atual] = 'a' + i;
            buffer_palavra[comprimento_atual + 1] = '\0';
            coletar_sugestoes(no_atual->ponteiros[i], buffer_palavra, comprimento_atual + 1, res, max_comprimento, visited);
        }
    }
}

int main(int argc, char* argv[]) {
    FILE* input = fopen(argv[1], "r");
    FILE* output = fopen(argv[2], "w");
    
    uint32_t quantidade_termos;
    fscanf(input, "%u", &quantidade_termos);
    char** vetor_termos = (char**)malloc(sizeof(char*) * quantidade_termos);
    for (uint32_t i = 0; i < quantidade_termos; i++) {
        vetor_termos[i] = (char*)malloc(sizeof(char) * 21);
        fscanf(input, "%20s", vetor_termos[i]);
    }

    uint32_t quantidade_requisicoes;
    fscanf(input, "%u", &quantidade_requisicoes);
    char** vetor_requisicoes = (char**)malloc(sizeof(char*) * quantidade_requisicoes);
    for (uint32_t i = 0; i < quantidade_requisicoes; i++) {
        vetor_requisicoes[i] = (char*)malloc(sizeof(char) * 21);
        fscanf(input, "%20s", vetor_requisicoes[i]);
    }

    no* nos = NULL;
    for (uint32_t i = 0; i < quantidade_termos; i++) {
        insercao(&nos, vetor_termos[i], strlen(vetor_termos[i]), 0, i);
    }

    char* visited = calloc(quantidade_termos, sizeof(char)); 

    for (uint32_t i = 0; i < quantidade_requisicoes; i++) {
        fprintf(output, "%s:", vetor_requisicoes[i]);

        Resultados resultados_finais;
        inicializar_resultados(&resultados_finais);

        uint32_t comprimento_req = strlen(vetor_requisicoes[i]);
        char prefixo_atual[25];
        strcpy(prefixo_atual, ""); 

        memset(visited, 0, quantidade_termos); 

        for (uint32_t j = 1; j <= comprimento_req; j++) {
            prefixo_atual[j - 1] = vetor_requisicoes[i][j - 1];
            prefixo_atual[j] = '\0';

            no* no_de_partida = encontrar_no_prefixo(nos, prefixo_atual);

            if (no_de_partida != NULL) {
                coletar_sugestoes(no_de_partida, prefixo_atual, j, &resultados_finais, j * 2, visited);
            }
        }

        if (resultados_finais.count > 0) {
            fprintf(output, "%s", vetor_termos[resultados_finais.indices[0]]);
            for (uint32_t j = 1; j < resultados_finais.count; j++) {
                fprintf(output, ",%s", vetor_termos[resultados_finais.indices[j]]);
            }
        } else {
            fprintf(output, "-");
        }
        fprintf(output, "\n");
    }

    return 0;
}