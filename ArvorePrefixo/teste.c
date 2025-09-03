#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Para compilar e executar:
// gcc -Wall -O2 seu_arquivo.c -o autocompletar
// ./autocompletar input.txt output.txt
// (Adicionada a flag -O2 para otimizações do compilador)

// --- ESTRUTURA DA TRIE OTIMIZADA ---
typedef struct no {
    struct no** ponteiros;
    uint32_t* valor;
    // Otimização: ID da última requisição que visitou este nó como palavra final.
    // Isso evita ter que buscar em uma lista de resultados para garantir unicidade.
    uint32_t id_ultima_requisicao;
} no;

// Estrutura para coletar os resultados finais
typedef struct {
    char** palavras;
    uint32_t count;
    uint32_t capacity;
} Resultados;

// --- FUNÇÕES BÁSICAS DA TRIE ---

// Aloca memória para um novo nó da árvore
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
    // Inicializa o ID com um valor que nunca será usado (UINT32_MAX é o maior inteiro sem sinal)
    criado->id_ultima_requisicao = UINT32_MAX;
    for (int i = 0; i < 26; i++) {
        criado->ponteiros[i] = NULL;
    }
    return criado;
}

// Libera recursivamente toda a memória usada pela árvore
void liberar_arvore(no* nos) {
    if (nos == NULL) return;
    for (int i = 0; i < 26; i++) {
        liberar_arvore(nos->ponteiros[i]);
    }
    if (nos->valor != NULL) free(nos->valor);
    free(nos->ponteiros);
    free(nos);
}

// Retorna o índice (0-25) para um caractere ('a'-'z')
uint32_t indice(const char* palavra, uint32_t profundidade) {
    return (uint32_t)(palavra[profundidade] - 'a');
}

// Insere uma palavra na árvore
void insercao(no** base, char* palavra_inserida, uint32_t tamanho_palavra, uint32_t profundidade, uint32_t novo_valor) {
    if (*base == NULL) *base = criar_no();
    if (profundidade == tamanho_palavra) {
        (*base)->valor = (uint32_t*)malloc(sizeof(uint32_t));
        *(*base)->valor = novo_valor;
        return;
    }
    insercao(&(*base)->ponteiros[indice(palavra_inserida, profundidade)], palavra_inserida, tamanho_palavra, profundidade + 1, novo_valor);
}

// --- FUNÇÕES DE COLETA E BUSCA OTIMIZADAS ---

// Funções para gerenciar a lista de resultados
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

// Encontra e retorna o nó que corresponde ao final de um prefixo
no* encontrar_no_prefixo(no* base, const char* prefixo) {
    no* atual = base;
    for (int i = 0; prefixo[i] != '\0'; i++) {
        if (atual == NULL) return NULL;
        atual = atual->ponteiros[indice(prefixo, i)];
    }
    return atual;
}

// Função de coleta totalmente otimizada
void coletar_sugestoes_otimizado(no* no_atual, char* buffer_palavra, uint32_t comprimento_atual, Resultados* res_finais, uint32_t max_comprimento, uint32_t id_req_atual) {
    if (no_atual == NULL) return;
    
    // Otimização: Poda usando o comprimento passado como parâmetro, em vez de strlen()
    if (comprimento_atual > max_comprimento) return;

    // Otimização: Verificação O(1) de unicidade usando o ID do nó
    if (no_atual->valor != NULL && no_atual->id_ultima_requisicao != id_req_atual) {
        adicionar_palavra(res_finais, buffer_palavra);
        no_atual->id_ultima_requisicao = id_req_atual; // Marca o nó como "visto" para esta requisição
    }

    for (int i = 0; i < 26; i++) {
        if (no_atual->ponteiros[i] != NULL) {
            buffer_palavra[comprimento_atual] = 'a' + i;
            buffer_palavra[comprimento_atual + 1] = '\0';
            coletar_sugestoes_otimizado(no_atual->ponteiros[i], buffer_palavra, comprimento_atual + 1, res_finais, max_comprimento, id_req_atual);
            buffer_palavra[comprimento_atual] = '\0'; // Backtrack
        }
    }
}

// --- FUNÇÃO PRINCIPAL ---

int main(int argc, char* argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Uso: %s <arquivo_de_entrada> <arquivo_de_saida>\n", argv[0]);
        return 1;
    }
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

    // Leitura dos dados
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

    // Construção da árvore
    no* nos = NULL;
    for (uint32_t i = 0; i < quantidade_termos; i++) {
        insercao(&nos, vetor_termos[i], strlen(vetor_termos[i]), 0, i);
    }

    // Processamento de cada requisição com a lógica otimizada
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
                // Passa o ID da requisição (i) e o comprimento do prefixo (j)
                coletar_sugestoes_otimizado(no_de_partida, prefixo_atual, j, &resultados_finais, j * 2, i);
            }
        }

        // Impressão dos resultados únicos e na ordem correta
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

    // Liberação de toda a memória alocada
    for (uint32_t i = 0; i < quantidade_requisicoes; i++) free(vetor_requisicoes[i]);
    free(vetor_requisicoes);
    for (uint32_t i = 0; i < quantidade_termos; i++) free(vetor_termos[i]);
    free(vetor_termos);
    liberar_arvore(nos);
    fclose(input);
    fclose(output);
    
    return 0;
}