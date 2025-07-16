#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

//gcc -Wall ArvoreAVL.c -o ArvoreAVL
//./ArvoreAVL input.txt output.txt

typedef struct Palavras{
char nome[31], sinonimos[10][31];
uint32_t quantidade_sinonimos;
} Palavras;

typedef struct Arvore{
Palavras* palavra_no;
int8_t altura;
struct Arvore* Esquerda;
struct Arvore* Direita;
} Arvore;

Arvore* cria_arv_vazia () {
   return NULL;
}

int maximo(int a, int b){
    if(a > b) return a;
    else return b;
}

int altura(Arvore* arvore){
    return (arvore == NULL) ? -1 : arvore->altura;
}

void atualiza_altura(Arvore* arvore){
    if(arvore != NULL) {
        int altura_esq = altura(arvore->Esquerda);
        int altura_dir = altura(arvore->Direita);
        arvore->altura = 1 +  maximo(altura_esq, altura_dir);
    }
}

int balanceamento(Arvore* arvore){
    if(arvore != NULL) return altura(arvore->Esquerda) - altura(arvore->Direita);
    return 0;
}

Arvore* rotacao_Esquerda(Arvore* raiz) {
    Arvore* eixo = raiz->Direita;
    raiz->Direita = eixo->Esquerda;
    eixo->Esquerda = raiz;
    atualiza_altura(raiz);
    atualiza_altura(eixo);
    return eixo;
}

Arvore* rotacao_Direita(Arvore* raiz) {
    Arvore* eixo = raiz->Esquerda;
    raiz->Esquerda = eixo->Direita;
    eixo->Direita = raiz;
    atualiza_altura(raiz);
    atualiza_altura(eixo);
    return eixo;
}

Arvore* rotacao_Esquerda_Direita(Arvore* raiz){
    raiz->Esquerda = rotacao_Esquerda(raiz->Esquerda);
    return rotacao_Direita(raiz);
}

Arvore* rotacao_Direita_Esquerda(Arvore* raiz){
    raiz->Direita = rotacao_Direita(raiz->Direita);
    return rotacao_Esquerda(raiz);
}

void buscar_imprimir(Arvore *a, char* palavra_inserida, FILE* output){
    if(a == NULL){
        fprintf(output, "?]\n-\n");
        return;
    }

    fprintf(output, "%s", a->palavra_no->nome);

    if(strcmp(a->palavra_no->nome, palavra_inserida) == 0){
        fprintf(output, "]\n");
        for(int i = 0; i < a->palavra_no->quantidade_sinonimos; i++){
            fprintf(output, "%s", a->palavra_no->sinonimos[i]);
            if (i < a->palavra_no->quantidade_sinonimos - 1) {
                fprintf(output, ",");
            }
        }
        fprintf(output, "\n");
    }

    else if(strcmp(a->palavra_no->nome, palavra_inserida) > 0){
        fprintf(output, "->");
        buscar_imprimir(a->Esquerda, palavra_inserida, output);
    }

    else if(strcmp(a->palavra_no->nome, palavra_inserida) < 0){
        fprintf(output, "->");
        buscar_imprimir(a->Direita, palavra_inserida, output);
    }
}

Arvore* Inserir_No(Arvore* a, Palavras palavra_inserida){
    if(a == NULL){
        a = (Arvore*) malloc(sizeof(Arvore));
        a->palavra_no = (Palavras*) malloc(sizeof(Palavras));
        if(a->palavra_no == 0){
            printf("Erro ao alocar o no");
            exit(1);
        }
        strcpy(a->palavra_no->nome, palavra_inserida.nome);
        a->palavra_no->quantidade_sinonimos = palavra_inserida.quantidade_sinonimos;
        for(int i = 0; i < palavra_inserida.quantidade_sinonimos; i++){
            strcpy(a->palavra_no->sinonimos[i], palavra_inserida.sinonimos[i]);
        }

        a->altura = 0;
        a->Esquerda = NULL;
        a->Direita = NULL;
        return a;
    }

    else if(strcmp(a->palavra_no->nome, palavra_inserida.nome) > 0){
        a->Esquerda = Inserir_No(a->Esquerda, palavra_inserida);
    }

    else if(strcmp(a->palavra_no->nome, palavra_inserida.nome) < 0){
        a->Direita = Inserir_No(a->Direita, palavra_inserida);
    }

    else{
        return a;
    }


    atualiza_altura(a);
    int8_t fator_balanceamento = balanceamento(a);
    if (fator_balanceamento > 1 && strcmp(palavra_inserida.nome, a->Esquerda->palavra_no->nome) < 0) {
        return rotacao_Direita(a);
    }

    if (fator_balanceamento < -1 && strcmp(palavra_inserida.nome, a->Direita->palavra_no->nome) > 0) {
        return rotacao_Esquerda(a);
    }

    if (fator_balanceamento > 1 && strcmp(palavra_inserida.nome, a->Esquerda->palavra_no->nome) > 0) {
        return rotacao_Esquerda_Direita(a);
    }

    if (fator_balanceamento < -1 && strcmp(palavra_inserida.nome, a->Direita->palavra_no->nome) < 0) {
        return rotacao_Direita_Esquerda(a);
    }

    return a;
}

void libera_arvore(Arvore* a) {
    if (a != NULL) {
        libera_arvore(a->Esquerda);
        libera_arvore(a->Direita);
        free(a->palavra_no);
        free(a);
    }
}

int main(int argc, char* argv[]){
    FILE* input = fopen(argv[1], "r");
    if(input == NULL){
        perror("Erro ao abrir arquivo de input");
        exit(1);
    }
    FILE* output = fopen(argv[2], "w");
    if(output == NULL){
        perror("Erro ao abrir arquivo de output");
        exit(1);
    }

    int quantidade_palavras_dicionario;
    fscanf(input, "%d", &quantidade_palavras_dicionario);

    Palavras *vetor_dicionario;
    vetor_dicionario = (Palavras*) malloc(sizeof(Palavras) * quantidade_palavras_dicionario);
    for(int i = 0; i < quantidade_palavras_dicionario; i++){
        fscanf(input, "%30s %d ", vetor_dicionario[i].nome, &vetor_dicionario[i].quantidade_sinonimos);
        for(int j = 0; j < vetor_dicionario[i].quantidade_sinonimos; j++){
            fscanf(input, "%30s", vetor_dicionario[i].sinonimos[j]);
        }
    }

    int quantidade_palavras_busca;
    fscanf(input, "%d", &quantidade_palavras_busca);

    char **vetor_busca;
    vetor_busca = (char**) malloc(sizeof(char*) * quantidade_palavras_busca);
    for(int i = 0; i < quantidade_palavras_busca; i++){
        vetor_busca[i] = (char*) malloc(sizeof(char) * 31);
        fscanf(input, "%30s", vetor_busca[i]);
    }

    Arvore *arvore_AVL = cria_arv_vazia();
    for(int i = 0; i < quantidade_palavras_dicionario; i++){
        arvore_AVL = Inserir_No(arvore_AVL, vetor_dicionario[i]);
    }

    for(int i = 0; i < quantidade_palavras_busca; i++){
        fprintf(output, "[");
        buscar_imprimir(arvore_AVL, vetor_busca[i], output);
    }

    
    fclose(input);
    fclose(output);
    free(vetor_dicionario);
    for(int i = 0; i < quantidade_palavras_busca; i++){
        free(vetor_busca[i]);
    }
    free(vetor_busca);
    libera_arvore(arvore_AVL);
    return 0;
}