#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h> 

//gcc -Wall arvoreprefixo.c -o arvoreprefixo
//./arvoreprefixo input.txt output.txt

typedef struct no {
    struct no** ponteiros;
    uint32_t* valor;
} no;

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
    }
    else {
        insercao(&(*base)->ponteiros[indice(palavra_inserida, profundidade)], palavra_inserida, strlen(palavra_inserida), profundidade + 1, novo_valor);
    }
}

no* encontrar_no_prefixo(no* base, char* palavra_base, uint32_t* profundidade_encontrada) {
    if (base == NULL) {
        *profundidade_encontrada = 0;
        return NULL;
    }

    no* atual = base;
    *profundidade_encontrada = 0;
    uint32_t tamanho_palavra = strlen(palavra_base);

    for (uint32_t i = 0; i < tamanho_palavra; i++) {
        uint32_t idx = indice(palavra_base, i);
        if (atual->ponteiros[idx] == NULL) {
            return atual;
        }
        atual = atual->ponteiros[idx];
        (*profundidade_encontrada)++;
    }
    return atual;
}

void coletar_palavras(no* no_atual, char* buffer_palavra, uint32_t max_comprimento, FILE* output, int* eh_primeira_palavra) {
    if (no_atual == NULL) {
        return;
    }

    if (no_atual->valor != NULL) {
        if (strlen(buffer_palavra) <= max_comprimento) {
            
            // LÓGICA DA VÍRGULA
            if (*eh_primeira_palavra == 1) {
                // Se for a primeira palavra, não imprima vírgula antes
                *eh_primeira_palavra = 0; // Atualiza a flag
            } else {
                // Para todas as outras, imprima a vírgula ANTES da palavra
                fprintf(output, ",");
            }
            
            fprintf(output, "%s", buffer_palavra); // Imprime a palavra sem vírgula depois
        }
    }

    uint32_t comprimento_atual = strlen(buffer_palavra);
    if (comprimento_atual >= max_comprimento) {
        return;
    }

    for (int i = 0; i < 26; i++) {
        if (no_atual->ponteiros[i] != NULL) {
            buffer_palavra[comprimento_atual] = 'a' + i;
            buffer_palavra[comprimento_atual + 1] = '\0';
            coletar_palavras(no_atual->ponteiros[i], buffer_palavra, max_comprimento, output, eh_primeira_palavra);
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
        fscanf(input, "%s", vetor_termos[i]);
    }

    uint32_t quantidade_requisicoes;
    fscanf(input, "%u", &quantidade_requisicoes);

    char** vetor_requisicoes = (char**)malloc(sizeof(char*) * quantidade_requisicoes);
    for (uint32_t i = 0; i < quantidade_requisicoes; i++) {
        vetor_requisicoes[i] = (char*)malloc(sizeof(char) * 21);
        fscanf(input, "%s", vetor_requisicoes[i]);
    }

    no* nos = NULL;

    for (uint32_t i = 0; i < quantidade_termos; i++) {
        insercao(&nos, vetor_termos[i], strlen(vetor_termos[i]), 0, i);
    }

    for (uint32_t i = 0; i < quantidade_requisicoes; i++) {
    uint32_t comprimento_prefixo_comum = 0;
    char* requisicao_atual = vetor_requisicoes[i];
    fprintf(output, "%s:", requisicao_atual);

    no* no_do_prefixo = encontrar_no_prefixo(nos, requisicao_atual, &comprimento_prefixo_comum);

    int eh_primeira_palavra = 1;

    if (no_do_prefixo != NULL && comprimento_prefixo_comum > 0) {
        uint32_t max_len = 2 * comprimento_prefixo_comum;
        char buffer_palavra[21];
        strncpy(buffer_palavra, requisicao_atual, comprimento_prefixo_comum);
        buffer_palavra[comprimento_prefixo_comum] = '\0';
        
        coletar_palavras(no_do_prefixo, buffer_palavra, max_len, output, &eh_primeira_palavra);
    }
    
    if (eh_primeira_palavra == 1) {
        fprintf(output, "-");
    }
    
    fprintf(output, "\n");
    }

    for (uint32_t i = 0; i < quantidade_requisicoes; i++) {
        free(vetor_requisicoes[i]);
    }
    for (uint32_t i = 0; i < quantidade_termos; i++) {
        free(vetor_termos[i]);
    }
    free(vetor_requisicoes);
    free(vetor_termos);
    liberar_arvore(nos);
    fclose(input);
    fclose(output);
    return 0;
}