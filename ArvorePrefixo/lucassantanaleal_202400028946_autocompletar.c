#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//gcc -Wall lucassantanaleal_202400028946_autocompletar.c -o lucassantanaleal_202400028946_autocompletar
//./lucassantanaleal_202400028946_autocompletar input.txt output.txt

typedef struct no {
    struct no** ponteiros;
    uint32_t* valor; 
} no;

typedef struct {
    char** palavras;
    uint32_t count;
    uint32_t capacity;
} Resultados;

no* criar_no() {
    no* criado = (no*)malloc(sizeof(no));
    if (criado == NULL) {
        perror("Nao foi possivel alocar espaco para o no");
        exit(1);
    }

    criado->ponteiros = (no**)malloc(sizeof(no*) * 26);
    if (criado->ponteiros == NULL) {
        perror("Nao foi possivel alocar espaco para os ponteiros do no");
        exit(1);
    }

    criado->valor = NULL;
    for (int i = 0; i < 26; i++) {
        criado->ponteiros[i] = NULL;
    }
    return criado;
}

void liberar_arvore(no* nos) {
    if (nos == NULL) {
        return;
    }
    for (int i = 0; i < 26; i++) {
        liberar_arvore(nos->ponteiros[i]);
    }
    if (nos->valor != NULL) {
        free(nos->valor);
    }
    free(nos->ponteiros);
    free(nos);
}

uint32_t indice(char* palavra, uint32_t profundidade) {
    char caractere_atual = palavra[profundidade];
    return (uint32_t)(caractere_atual - 'a');
}

void insercao(no** base, char* palavra_inserida, uint32_t tamanho_palavra, uint32_t profundidade, uint32_t novo_valor) {
    if (*base == NULL) *base = criar_no();

    if (profundidade == tamanho_palavra) {
        (*base)->valor = (uint32_t*)malloc(sizeof(uint32_t));
        *(*base)->valor = novo_valor;
        return;
    } else {
        insercao(&(*base)->ponteiros[indice(palavra_inserida, profundidade)], palavra_inserida, tamanho_palavra, profundidade + 1, novo_valor);
    }
}

void inicializar_resultados(Resultados* res) {
    res->count = 0;
    res->capacity = 20;
    res->palavras = malloc(sizeof(char*) * res->capacity);
}

void adicionar_palavra(Resultados* res, const char* palavra) {
    if (res->count >= res->capacity) {
        res->capacity *= 2;
        res->palavras = realloc(res->palavras, sizeof(char*) * res->capacity);
    }
    res->palavras[res->count++] = strdup(palavra);
}

void liberar_resultados(Resultados* res) {
    for (uint32_t i = 0; i < res->count; i++) free(res->palavras[i]);
    free(res->palavras);
}

int contem_palavra(Resultados* res, const char* palavra) {
    for (uint32_t i = 0; i < res->count; i++) {
        if (strcmp(res->palavras[i], palavra) == 0) {
            return 1; // True
        }
    }
    return 0; // False
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

void coletar_sugestoes(no* no_atual, char* buffer_palavra, Resultados* res, uint32_t max_comprimento) {
    if (no_atual == NULL || strlen(buffer_palavra) > max_comprimento) {
        return;
    }
    if (no_atual->valor != NULL) {
        adicionar_palavra(res, buffer_palavra);
    }

    uint32_t comprimento_atual = strlen(buffer_palavra);
    for (int i = 0; i < 26; i++) {
        if (no_atual->ponteiros[i] != NULL) {
            buffer_palavra[comprimento_atual] = 'a' + i;
            buffer_palavra[comprimento_atual + 1] = '\0';
            coletar_sugestoes(no_atual->ponteiros[i], buffer_palavra, res, max_comprimento);
            buffer_palavra[comprimento_atual] = '\0'; 
        }
    }
}

int main(int argc, char* argv[]) {
    FILE* input = fopen(argv[1], "r");
    if (input == NULL) {
        perror("Nao foi possivel abrir o arquivo de input");
        exit(1);
    }
    FILE* output = fopen(argv[2], "w");
    if (output == NULL) {
        perror("Nao foi possivel abrir o arquivo de output");
        fclose(input);
        exit(1);
    }

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

    for (uint32_t i = 0; i < quantidade_requisicoes; i++) {
        fprintf(output, "%s:", vetor_requisicoes[i]);

        Resultados resultados_finais;
        inicializar_resultados(&resultados_finais);

        uint32_t comprimento_req = strlen(vetor_requisicoes[i]);
        char prefixo_atual[25];

        for (uint32_t j = 1; j <= comprimento_req; j++) {
            strncpy(prefixo_atual, vetor_requisicoes[i], j);
            prefixo_atual[j] = '\0';

            no* no_de_partida = encontrar_no_prefixo(nos, prefixo_atual);
            
            if (no_de_partida != NULL) {
                Resultados resultados_temporarios;
                inicializar_resultados(&resultados_temporarios);

                coletar_sugestoes(no_de_partida, prefixo_atual, &resultados_temporarios, j * 2);

                for (uint32_t k = 0; k < resultados_temporarios.count; k++) {
                    if (!contem_palavra(&resultados_finais, resultados_temporarios.palavras[k])) {
                        adicionar_palavra(&resultados_finais, resultados_temporarios.palavras[k]);
                    }
                }
                liberar_resultados(&resultados_temporarios);
            }
        }

        if (resultados_finais.count > 0) {
            fprintf(output, "%s", resultados_finais.palavras[0]);
            for (uint32_t j = 1; j < resultados_finais.count; j++) {
                fprintf(output, ",%s", resultados_finais.palavras[j]);
            }
        } else {
            fprintf(output, "-");
        }
        fprintf(output, "\n");

        liberar_resultados(&resultados_finais);
    }

    return 0;
}