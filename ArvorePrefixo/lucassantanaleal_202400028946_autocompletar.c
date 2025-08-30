#include <inttypes.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

//gcc -Wall lucassantanaleal_202400028946_autocompletar.c -o lucassantanaleal_202400028946_autocompletar
//./lucassantanaleal_202400028946_autocompletar input.txt output.txt

typedef struct no{
    struct no** ponteiros;
    uint32_t* valor;
} no;

no* criar_no(){
    no* criado = (no*) malloc(sizeof(no));
    if(criado == NULL){
        perror("Nao foi possivel alocar espaco para o no");
        exit(1);
    }
    
    criado->ponteiros = (no**) malloc(sizeof(no*) * 26);
    if(criado->ponteiros == NULL){
        perror("Nao foi possivel alocar espaco para os ponteiros do no");
        exit(1);
    }

    criado->valor = NULL;

    for(int i = 0; i < 26; i++){
        criado->ponteiros[i] = NULL;
    }

    return criado;
}

void liberar_arvore(no* nos){
    if(nos == NULL){
        return;
    }

    else{
        for(int i = 0; i < 26; i++){
            liberar_arvore(nos->ponteiros[i]);
        }
    }
    if(nos->valor != NULL){
        free(nos->valor);
    }
    free(nos->ponteiros);
    free(nos);
}

uint32_t indice(char* palavra, uint32_t profundidade){
    char caractere_atual = palavra[profundidade];
    return (uint32_t) (caractere_atual - 'a');
}

uint32_t calcular_prefixo_comum(const char* str1, const char* str2){
    uint32_t comprimento = 0;
    while(str1[comprimento] != '\0' && str2[comprimento] != '\0' && str1[comprimento] == str2[comprimento]){
        comprimento++;
    }
    return comprimento;
}

void verificar_e_coletar_palavras(no* no_atual, char* buffer_palavra, uint32_t profundidade, const char* palavra_requisicao, FILE* output, int* eh_primeira_palavra){
    if(no_atual == NULL){
        return;
    }

    uint32_t L = calcular_prefixo_comum(buffer_palavra, palavra_requisicao);

    if(profundidade > (2 * L) && L < strlen(palavra_requisicao)){
        return;
    }

    if(no_atual->valor != NULL){ //onde ele deixa de ser nulo? preciso depurar, alguma outra lógica associada ao ponteiro

        if(L > 0 && strlen(buffer_palavra) <= (2 * L)){  
            if(*eh_primeira_palavra == 1){
                *eh_primeira_palavra = 0;
            } 
            else{
                fprintf(output, ",");
            }
            fprintf(output, "%s", buffer_palavra);
        }
    }

    uint32_t comprimento_atual = strlen(buffer_palavra);
    for(int i = 0; i < 26; i++){
        if(no_atual->ponteiros[i] != NULL){
            buffer_palavra[comprimento_atual] = 'a' + i;
            buffer_palavra[comprimento_atual + 1] = '\0';
            
            verificar_e_coletar_palavras(no_atual->ponteiros[i], buffer_palavra, profundidade + 1, palavra_requisicao, output, eh_primeira_palavra);
            
            buffer_palavra[comprimento_atual] = '\0';
        }
    }
}

void insercao(no** base, char* palavra_inserida, uint32_t tamanho_palavra, uint32_t profundidade, uint32_t novo_valor){
    if(*base == NULL) *base = criar_no();

    if(profundidade == tamanho_palavra){
        (*base)->valor = (uint32_t*) malloc(sizeof(uint32_t));
        *(*base)->valor = novo_valor;
        return;
    }
    else{
        insercao(&(*base)->ponteiros[indice(palavra_inserida, profundidade)], palavra_inserida, tamanho_palavra, profundidade + 1, novo_valor);
    }
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

    uint32_t quantidade_termos;
    fscanf(input, "%u", &quantidade_termos);

    char** vetor_termos = (char**) malloc(sizeof(char*) * quantidade_termos);
    for(int i = 0; i < quantidade_termos; i++){
        vetor_termos[i] = (char*) malloc(sizeof(char) * 21);
        fscanf(input, "%20s", vetor_termos[i]);
    }

    uint32_t quantidade_requisicoes;
    fscanf(input, "%u", &quantidade_requisicoes);

    char** vetor_requisicoes = (char**) malloc(sizeof(char*) * quantidade_requisicoes);
    for(int i = 0; i < quantidade_requisicoes; i++){
        vetor_requisicoes[i] = (char*) malloc(sizeof(char) * 21);
        fscanf(input, "%20s", vetor_requisicoes[i]);
    }

    no* nos = NULL;

    for(int i = 0; i < quantidade_termos; i++){
        insercao(&nos, vetor_termos[i], strlen(vetor_termos[i]), 0, i);
    }

    for(int i = 0; i < quantidade_requisicoes; i++){
    fprintf(output, "%s:", vetor_requisicoes[i]);

    char buffer_palavra[25] = ""; 
    int eh_primeira_palavra = 1;
    uint32_t profundidade = 0;

    verificar_e_coletar_palavras(nos, buffer_palavra, profundidade, vetor_requisicoes[i], output, &eh_primeira_palavra);
    if(eh_primeira_palavra == 1){
        fprintf(output, "-");
    }
    
    fprintf(output, "\n");
    }

    for(int i = 0; i < quantidade_requisicoes; i++){
        free(vetor_requisicoes[i]);
    }
    for(int i = 0; i < quantidade_termos; i++){
        free(vetor_termos[i]);
    }

    /* free(vetor_requisicoes);
    free(vetor_termos);
    liberar_arvore(nos);
    fclose(input);
    fclose(output); */
    return 0;
}
