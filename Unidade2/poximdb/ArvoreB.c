#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

//gcc -Wall ArvoreB.c -o ArvoreB
//./ArvoreB input.txt output.txt

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
    for(int i = 0; i < grau_maximo_arvore; i++){
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

void divisao_no(no_BTree* no_pai, int indice, uint32_t grau_maximo_arvore){
    uint32_t indice_mediana = grau_maximo_arvore / 2; //se o grau for 3, o nó terá no max 3 chaves, quando tiver c0, c1, c2, c1 será a mediana
    no_BTree* no_filho = no_pai->Ponteiros[indice]; //o nó já existe, eu só referencio ele
    no_BTree* novo_no = criar_no(no_filho->eh_folha, grau_maximo_arvore);

    novo_no->quantidade_chaves_inseridas = grau_maximo_arvore - indice_mediana - 1; 

    for(int i = 0; i < novo_no->quantidade_chaves_inseridas; i++){
        novo_no->chaves_inseridas[i] = no_filho->chaves_inseridas[indice_mediana + 1 + i]; //transfere a metade direita do antigo nó cheio ao nó novo
    }

    if(!no_filho->eh_folha){
        for(int i = 0; i <= novo_no->quantidade_chaves_inseridas; i++){   
            novo_no->Ponteiros[i] = no_filho->Ponteiros[indice_mediana + 1 + i]; //da mesma forma que a transferencia de cima, deve começar do indice logo depois da mediana
        }
    }

    no_filho->quantidade_chaves_inseridas = indice_mediana; //atualiza a quantidade de chaves do nó que foi dividido

    for(int i = no_pai->quantidade_chaves_inseridas; i >= indice + 1; i--){
        no_pai->Ponteiros[i + 1] = no_pai->Ponteiros[i];
    }

    no_pai->Ponteiros[indice + 1] = novo_no;

    for(int i = no_pai->quantidade_chaves_inseridas - 1; i >= indice; i--){
        no_pai->chaves_inseridas[i + 1] = no_pai->chaves_inseridas[i];
    }

    no_pai->chaves_inseridas[indice] = no_filho->chaves_inseridas[indice_mediana];
    no_pai->quantidade_chaves_inseridas++;
}

void inserir_parcialmente_cheio(no_BTree* no, arquivo arquivo_inserido, uint32_t grau_maximo_arvore){
    int i = no->quantidade_chaves_inseridas - 1;

    if(no->eh_folha){ //esse bloco faz empurrar as chaves do nó pra esquerda, abrindo espaço pra nova chave que será inserida, no caso de ser nó folha
        while(i >= 0 && strcmp(no->chaves_inseridas[i].chave_hash, arquivo_inserido.chave_hash) > 0){
            no->chaves_inseridas[i + 1] = no->chaves_inseridas[i];
            i--;
        }
        no->chaves_inseridas[i + 1] = arquivo_inserido;
        no->quantidade_chaves_inseridas++;
    }

    else{
        while(i >= 0 && strcmp(no->chaves_inseridas[i].chave_hash, arquivo_inserido.chave_hash) > 0){ //procura qual ponteiro seguir
            i--;
        }
        i++;

        if(no->Ponteiros[i]->quantidade_chaves_inseridas == grau_maximo_arvore){
            divisao_no(no, i, grau_maximo_arvore);
            if(strcmp(no->chaves_inseridas[i].chave_hash, arquivo_inserido.chave_hash) < 0){
                i++;
            }
        }
        inserir_parcialmente_cheio(no->Ponteiros[i], arquivo_inserido, grau_maximo_arvore);
    }
}

void inserir(no_BTree** raiz, arquivo arquivo_inserido, uint32_t grau_maximo_arvore){
    no_BTree* no = *raiz;

    if(no == NULL){ //caso estejamos criando a árvore
        *raiz = criar_no(true, grau_maximo_arvore);
        (*raiz)->chaves_inseridas[0] = arquivo_inserido;
        (*raiz)->quantidade_chaves_inseridas = 1;
        return;
    }

    else{
        if(no->quantidade_chaves_inseridas == grau_maximo_arvore){
            no_BTree* nova_raiz = criar_no(false, grau_maximo_arvore);
            nova_raiz->Ponteiros[0] = no;
            divisao_no(nova_raiz, 0, grau_maximo_arvore);
            *raiz = nova_raiz;
        }
        inserir_parcialmente_cheio(*raiz, arquivo_inserido, grau_maximo_arvore);
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