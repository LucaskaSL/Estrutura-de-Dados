#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

//gcc -Wall lucassantanaleal_202400028946_poximdb.c -o lucassantanaleal_202400028946_poximdb
//./lucassantanaleal_202400028946_poximdb poximdb.input poximdb.output

typedef struct arquivo{
    char nome[31]; //definido no input, simplesmente existe por pura estética para o output, não evaluemos nada nele
    uint32_t tamanho; //definido no input, simplesmente existe por pura estética para o output, não evaluemos nada nele
    char chave_hash[33];
} arquivo;

typedef struct no_BTree{
    arquivo* chaves_inseridas;
    uint32_t quantidade_chaves_inseridas;
    struct no_BTree** Ponteiros;
    bool eh_folha;
}no_BTree;

no_BTree* criar_no(bool eh_folha, uint32_t grau_maximo_arvore){
    no_BTree* novo_no = (no_BTree*) malloc(sizeof(no_BTree));
    if(novo_no == NULL){
        perror("Erro ao alocar memoria para o no");
        exit(1);
    }

    novo_no->chaves_inseridas = (arquivo*) malloc(sizeof(arquivo) * grau_maximo_arvore);
    novo_no->Ponteiros = (no_BTree**) malloc(sizeof(no_BTree*) * (grau_maximo_arvore + 1));
    if(novo_no->chaves_inseridas == NULL || novo_no->Ponteiros == NULL){
        perror("Erro ao alocar memoria para os vetores do no");
        exit(1);
    }

    novo_no->eh_folha = eh_folha;
    novo_no->quantidade_chaves_inseridas = 0;
    for(int i = 0; i < grau_maximo_arvore + 1; i++){
        novo_no->Ponteiros[i] = NULL;
    }
    
    return novo_no;
}

void destruir_arvore(no_BTree* arvore){
    if (arvore == NULL) {
        return;
    }

    if(!arvore->eh_folha){
        for (int i = 0; i <= arvore->quantidade_chaves_inseridas; i++) {
            destruir_arvore(arvore->Ponteiros[i]);
        }
    }

    free(arvore->chaves_inseridas);
    free(arvore->Ponteiros);
    free(arvore);
}

no_BTree* busca(no_BTree* no, arquivo arquivo_desejado, FILE* output){
    no_BTree* r = NULL;
    if(no != NULL) {
        uint32_t i = 0;
        while(i < no->quantidade_chaves_inseridas && strcmp(arquivo_desejado.chave_hash, no->chaves_inseridas[i].chave_hash) > 0){
            i++;
        }
        if(i < no->quantidade_chaves_inseridas && strcmp(arquivo_desejado.chave_hash, no->chaves_inseridas[i].chave_hash) == 0){
            r = no;
        }
        else if(no->eh_folha == false){
            r = busca(no->Ponteiros[i], arquivo_desejado, output);
        }
    }
    return r;
}

void divisao(no_BTree** raiz, no_BTree* no_a_dividir, no_BTree* pai, int indice_no_pai, uint32_t grau_maximo_arvore);

void inserir(no_BTree** raiz, arquivo arquivo_inserido, uint32_t grau_maximo_arvore) {
    // Caso 1: Árvore vazia.
    if (*raiz == NULL) {
        *raiz = criar_no(true, grau_maximo_arvore);
        (*raiz)->chaves_inseridas[0] = arquivo_inserido;
        (*raiz)->quantidade_chaves_inseridas = 1;
        return;
    }

    // --- Passo 1: Descer até a folha guardando o caminho ---
    no_BTree* no_atual = *raiz;
    no_BTree* caminho_pais[100]; // Suporta uma árvore de altura 100
    int indices_caminho[100];
    int nivel = 0;

    while (!no_atual->eh_folha) {
        caminho_pais[nivel] = no_atual; // Guarda o pai

        int i = no_atual->quantidade_chaves_inseridas - 1;
        while (i >= 0 && strcmp(no_atual->chaves_inseridas[i].chave_hash, arquivo_inserido.chave_hash) > 0) {
            i--;
        }
        i++;
        indices_caminho[nivel] = i; // Guarda o índice do filho
        
        no_atual = no_atual->Ponteiros[i];
        nivel++;
    }

    // --- Passo 2: Inserir a chave diretamente na folha ---
    int i = no_atual->quantidade_chaves_inseridas - 1;
    while (i >= 0 && strcmp(no_atual->chaves_inseridas[i].chave_hash, arquivo_inserido.chave_hash) > 0) {
        no_atual->chaves_inseridas[i + 1] = no_atual->chaves_inseridas[i];
        i--;
    }
    no_atual->chaves_inseridas[i + 1] = arquivo_inserido;
    no_atual->quantidade_chaves_inseridas++;

    // --- Passo 3: Verificar e dividir, subindo pela árvore ---
    while (no_atual != NULL && no_atual->quantidade_chaves_inseridas == grau_maximo_arvore) {
        nivel--; 
        no_BTree* pai = (nivel < 0) ? NULL : caminho_pais[nivel];
        int indice_no_pai = (nivel < 0) ? 0 : indices_caminho[nivel];

        divisao(raiz, no_atual, pai, indice_no_pai, grau_maximo_arvore);
        
        no_atual = pai; // Move o foco para o pai, para verificar se ele agora precisa ser dividido.
    }
}

void divisao(no_BTree** raiz, no_BTree* no_cheio, no_BTree* pai, int indice_no_pai, uint32_t grau_maximo_arvore) {
    int indice_mediana = grau_maximo_arvore / 2;
    arquivo chave_promovida = no_cheio->chaves_inseridas[indice_mediana];
    no_BTree* novo_irmao = criar_no(no_cheio->eh_folha, grau_maximo_arvore);

    novo_irmao->quantidade_chaves_inseridas = grau_maximo_arvore - indice_mediana - 1;
    for (int i = 0; i < novo_irmao->quantidade_chaves_inseridas; i++) {
        novo_irmao->chaves_inseridas[i] = no_cheio->chaves_inseridas[i + indice_mediana + 1];
    }
    if (!no_cheio->eh_folha) {
        for (int i = 0; i <= novo_irmao->quantidade_chaves_inseridas; i++) {
            novo_irmao->Ponteiros[i] = no_cheio->Ponteiros[i + indice_mediana + 1];
        }
    }
    
    no_cheio->quantidade_chaves_inseridas = indice_mediana;
    
    if (pai == NULL) {
        pai = criar_no(false, grau_maximo_arvore);
        *raiz = pai;
        pai->Ponteiros[0] = no_cheio;
        // O índice onde a chave promovida entrará é 0
        indice_no_pai = 0; 
    }
    
    for (int i = pai->quantidade_chaves_inseridas; i > indice_no_pai; i--) {
        pai->chaves_inseridas[i] = pai->chaves_inseridas[i - 1];
        pai->Ponteiros[i + 1] = pai->Ponteiros[i];
    }
    pai->chaves_inseridas[indice_no_pai] = chave_promovida;
    pai->Ponteiros[indice_no_pai + 1] = novo_irmao;
    pai->quantidade_chaves_inseridas++;
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

    char linha[256];
    uint32_t grau_maximo_arvore;
    no_BTree* arvore = NULL;
    fgets(linha, sizeof(linha), input);
    sscanf(linha, "%u", &grau_maximo_arvore);

    int quantidade_arquivos;
    fgets(linha, sizeof(linha), input);
    sscanf(linha, "%d", &quantidade_arquivos);

    for(int i = 0; i < quantidade_arquivos; i++){
        if (!fgets(linha, sizeof(linha), input)) break;
        arquivo arquivo_temp;
        if (sscanf(linha, "%30s %u %32s", arquivo_temp.nome, &arquivo_temp.tamanho, arquivo_temp.chave_hash) == 3) {
            inserir(&arvore, arquivo_temp, grau_maximo_arvore);
        }
    }

    int quantidade_comandos;
    fgets(linha, sizeof(linha), input);
    sscanf(linha, "%d", &quantidade_comandos);

    for(int i = 0; i < quantidade_comandos; i++){
        if (!fgets(linha, sizeof(linha), input)) {
            break;
        }  

        char nome_comando[20]; 
        sscanf(linha, "%19s", nome_comando); 

        if(strcmp(nome_comando, "INSERT") == 0){
            arquivo arquivo_temp;
            if (sscanf(linha, "%*s %30s %u %32s", arquivo_temp.nome, &arquivo_temp.tamanho, arquivo_temp.chave_hash) == 3) {
                inserir(&arvore, arquivo_temp, grau_maximo_arvore);
            }
        }

        else if(strcmp(nome_comando, "SELECT") == 0){
            arquivo arquivo_temp;
            if (sscanf(linha, "%*s %32s", arquivo_temp.chave_hash) == 1) {
                no_BTree* no_encontrado = busca(arvore, arquivo_temp, output);

                if(no_encontrado != NULL){
                    fprintf(output, "[%s]\n", arquivo_temp.chave_hash);
                    for(int i = 0; i < no_encontrado->quantidade_chaves_inseridas; i++){ 
                        fprintf(output, "%s:size=%d,hash=%s\n", no_encontrado->chaves_inseridas[i].nome, 
                            no_encontrado->chaves_inseridas[i].tamanho,
                            no_encontrado->chaves_inseridas[i].chave_hash);
                    }
                }
                else{
                    fprintf(output, "[%s]\n", arquivo_temp.chave_hash);
                    fprintf(output, "-\n");
                }
            }        
        }
    }  
    
    fclose(input);
    fclose(output);
    destruir_arvore(arvore);
    return 0;
}