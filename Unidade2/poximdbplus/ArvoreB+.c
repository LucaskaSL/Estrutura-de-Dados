#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <stdbool.h>

//gcc -Wall ArvoreB+.c -o ArvoreB+
//./ArvoreB+ poximdbplus.input poximdbplus.output

typedef struct arquivo{
    char nome[31]; //definido no input, simplesmente existe por pura estética para o output, não evaluemos nada nele
    uint32_t tamanho; //definido no input, simplesmente existe por pura estética para o output, não evaluemos nada nele
    char chave_hash[33];
} arquivo;

typedef struct no_BplusTree{
    arquivo* chaves_inseridas;
    uint32_t quantidade_chaves_inseridas;
    struct no_BplusTree** Ponteiros;
    bool eh_folha;
    struct no_BplusTree* proximo_fila;
} no_BplusTree;

typedef struct Elemento_lista{
    no_BplusTree* no_armazenado;
    struct Elemento_lista* proximo;
} Elemento_lista;

typedef struct lista{
    Elemento_lista* inicio;
    Elemento_lista* fim;
    uint32_t quantidade_nos;
} lista;

arquivo maximo(arquivo a, arquivo b){
    if(strcmp(a.chave_hash, b.chave_hash) > 0){
        return a;
    }
    else{
        return b;
    }
}

arquivo minimo(arquivo a, arquivo b){
    if(strcmp(a.chave_hash, b.chave_hash) > 0){
        return b;
    }
    else{
        return a;
    }
}

no_BplusTree* criar_no(bool eh_folha, uint32_t grau_maximo_arvore){
    no_BplusTree* novo_no = (no_BplusTree*) malloc(sizeof(no_BplusTree));
    if(novo_no == NULL){
        perror("Erro ao alocar memoria para o no");
        exit(1);
    }

    novo_no->chaves_inseridas = (arquivo*) malloc(sizeof(arquivo) * grau_maximo_arvore);
    novo_no->Ponteiros = (no_BplusTree**) malloc(sizeof(no_BplusTree*) * (grau_maximo_arvore + 1));
    if(novo_no->chaves_inseridas == NULL || novo_no->Ponteiros == NULL){
        perror("Erro ao alocar memoria para os vetores do no");
        exit(1);
    }

    novo_no->eh_folha = eh_folha;
    novo_no->quantidade_chaves_inseridas = 0;
    for(int i = 0; i < grau_maximo_arvore + 1; i++){
        novo_no->Ponteiros[i] = NULL;
    }
    novo_no->proximo_fila = NULL;
    
    return novo_no;
}

lista* criar_lista(){
    lista* lista_criada = (lista*) malloc(sizeof(lista));
    lista_criada->fim = NULL;
    lista_criada->inicio = NULL;
    lista_criada->quantidade_nos = 0;

    return lista_criada;
}

lista* adicionar_nos_lista(lista* lista, no_BplusTree* no){
    Elemento_lista* elemento_inserido = (Elemento_lista*) malloc(sizeof(Elemento_lista));
    if(elemento_inserido == NULL){
        perror("Nao foi possivel alocar o elemento da lista");
        exit(1);
    }
    elemento_inserido->proximo = NULL;
    elemento_inserido->no_armazenado = no;

    if (lista->fim != NULL) lista->fim->proximo = elemento_inserido; 
    else lista->inicio = elemento_inserido; 
    lista->fim = elemento_inserido;
    lista->quantidade_nos++;
    
    return lista;
}

void liberar_lista(lista* lista){
    Elemento_lista* no_atual = lista->inicio;
    while(no_atual != NULL){
        Elemento_lista* temp = no_atual;
        no_atual = no_atual->proximo;
        free(temp);
    }
    free(lista);
}

void destruir_arvore(no_BplusTree* arvore){
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

no_BplusTree* busca_folha(no_BplusTree* no, arquivo chave_inicial){
    if(no == NULL){
        return NULL;
    }

    no_BplusTree* no_atual = no;

    while(no_atual->eh_folha == false){
        uint32_t i = 0;
        while(i < no_atual->quantidade_chaves_inseridas && strcmp(chave_inicial.chave_hash, no_atual->chaves_inseridas[i].chave_hash) > 0){
            i++;
        }
        no_atual = no_atual->Ponteiros[i];
    }

    return no_atual;
}

lista* busca_intervalo(no_BplusTree* x, arquivo a, arquivo b, uint32_t grau_maximo_arvore, FILE* output){
    arquivo inicio = minimo(a, b), fim = maximo(a, b);
    lista* r = criar_lista();
    no_BplusTree* f = busca_folha(x, inicio);
    while(f != NULL){
        // CONDIÇÃO DE PARADA: Se a primeira chave do nó já for MAIOR que o fim do intervalo, paramos.
        if(strcmp(f->chaves_inseridas[0].chave_hash, fim.chave_hash) > 0){
            break;
        }

        // CONDIÇÃO DE INCLUSÃO: Adicionamos o nó apenas se ele tiver sobreposição com o intervalo.
        // (A última chave do nó deve ser maior ou igual ao início do intervalo)
        if(strcmp(f->chaves_inseridas[f->quantidade_chaves_inseridas - 1].chave_hash, inicio.chave_hash) >= 0){
            adicionar_nos_lista(r, f);
        }

        f = f->proximo_fila;
    }
    return r;
}

no_BplusTree* busca(no_BplusTree* no, arquivo arquivo_desejado, FILE* output){
    no_BplusTree* r = NULL;
    if(no != NULL) {
        uint32_t i = 0;
        while(i < no->quantidade_chaves_inseridas && strcmp(arquivo_desejado.chave_hash, no->chaves_inseridas[i].chave_hash) > 0){
            i++;
        }
        if(i < no->quantidade_chaves_inseridas && strcmp(arquivo_desejado.chave_hash, no->chaves_inseridas[i].chave_hash) == 0 && no->eh_folha == true){
            r = no;
        }
        else if(no->eh_folha == false){
            r = busca(no->Ponteiros[i], arquivo_desejado, output);
        }
    }
    return r;
}

void divisao(no_BplusTree** raiz, no_BplusTree* no_a_dividir, no_BplusTree* pai, int indice_no_pai, uint32_t grau_maximo_arvore);

void inserir(no_BplusTree** raiz, arquivo arquivo_inserido, uint32_t grau_maximo_arvore) {
    if(*raiz == NULL){
        *raiz = criar_no(true, grau_maximo_arvore);
        (*raiz)->chaves_inseridas[0] = arquivo_inserido;
        (*raiz)->quantidade_chaves_inseridas = 1;
        return;
    }

    //Desce até a folha guardando o caminho
    no_BplusTree* no_atual = *raiz;
    no_BplusTree* caminho_pais[100]; //Suporta uma árvore de altura 100
    int indices_caminho[100];
    int nivel = 0;

    while(!no_atual->eh_folha){
        caminho_pais[nivel] = no_atual; //Guarda o pai

        int i = no_atual->quantidade_chaves_inseridas - 1;
        while(i >= 0 && strcmp(no_atual->chaves_inseridas[i].chave_hash, arquivo_inserido.chave_hash) > 0){
            i--;
        }
        i++;
        indices_caminho[nivel] = i; //Guarda o índice do filho
        
        no_atual = no_atual->Ponteiros[i];
        nivel++;
    }

    //Insere a chave diretamente na folha
    int i = no_atual->quantidade_chaves_inseridas - 1;
    while(i >= 0 && strcmp(no_atual->chaves_inseridas[i].chave_hash, arquivo_inserido.chave_hash) > 0){
        no_atual->chaves_inseridas[i + 1] = no_atual->chaves_inseridas[i];
        i--;
    }
    no_atual->chaves_inseridas[i + 1] = arquivo_inserido;
    no_atual->quantidade_chaves_inseridas++;

    //Verifica e divide, subindo pela árvore
    while(no_atual != NULL && no_atual->quantidade_chaves_inseridas == grau_maximo_arvore){
        nivel--; 
        no_BplusTree* pai = (nivel < 0) ? NULL : caminho_pais[nivel];
        int indice_no_pai = (nivel < 0) ? 0 : indices_caminho[nivel];

        divisao(raiz, no_atual, pai, indice_no_pai, grau_maximo_arvore);
        
        no_atual = pai; //Move o foco para o pai, para verificar se ele agora precisa ser dividido.
    }
}

void divisao(no_BplusTree** raiz, no_BplusTree* no_cheio, no_BplusTree* pai, int indice_no_pai, uint32_t grau_maximo_arvore){
    printf("Divisao chamada\n");
    printf("No a dividir tem %u chaves. Folha? %s\n", no_cheio->quantidade_chaves_inseridas, no_cheio->eh_folha ? "Sim" : "Nao");
    if(pai == NULL){
        pai = criar_no(false, grau_maximo_arvore);
        *raiz = pai;
        pai->Ponteiros[0] = no_cheio;
        // O índice onde a chave promovida entrará é 0
        indice_no_pai = 0; 
    }
    
    int indice_mediana = grau_maximo_arvore / 2;
    arquivo chave_promovida = no_cheio->chaves_inseridas[indice_mediana];
    no_BplusTree* novo_irmao = criar_no(no_cheio->eh_folha, grau_maximo_arvore); //irmão à direita

    
    if(!no_cheio->eh_folha){ //lógica para nós internos
        printf("Dividindo no FOLHA. Chave a ser promovida: %s\n", chave_promovida.chave_hash);
        printf("  Conteudo ANTES da divisao: ");
        for(int k=0; k < no_cheio->quantidade_chaves_inseridas; k++) {
            printf("%s ", no_cheio->chaves_inseridas[k].chave_hash);
        }
        printf("\n");

        novo_irmao->quantidade_chaves_inseridas = grau_maximo_arvore - indice_mediana - 1;
        for(int i = 0; i < novo_irmao->quantidade_chaves_inseridas; i++){
            novo_irmao->chaves_inseridas[i] = no_cheio->chaves_inseridas[i + indice_mediana + 1];
        }
        for(int i = 0; i <= novo_irmao->quantidade_chaves_inseridas; i++){
            novo_irmao->Ponteiros[i] = no_cheio->Ponteiros[i + indice_mediana + 1];
        }
        no_cheio->quantidade_chaves_inseridas = indice_mediana;

        printf("  Conteudo DEPOIS da divisao:\n");
        printf("    No antigo (esq): ");
        for(int k=0; k < no_cheio->quantidade_chaves_inseridas; k++) {
            printf("%s ", no_cheio->chaves_inseridas[k].chave_hash);
        }
        printf("\n    No novo (dir): ");
        for(int k=0; k < novo_irmao->quantidade_chaves_inseridas; k++) {
            printf("%s ", novo_irmao->chaves_inseridas[k].chave_hash);
        }
        printf("\n-----------------------\n");
    }
    
    else if(no_cheio->eh_folha){ //lógica para nós folhas
        printf("Dividindo no FOLHA. Chave a ser promovida: %s\n", chave_promovida.chave_hash);
        printf("  Conteudo ANTES da divisao: ");
        for(int k=0; k < no_cheio->quantidade_chaves_inseridas; k++) {
            printf("%s ", no_cheio->chaves_inseridas[k].chave_hash);
        }
        printf("\n");

        novo_irmao->quantidade_chaves_inseridas = grau_maximo_arvore - indice_mediana;
        for(int i = 0; i < novo_irmao->quantidade_chaves_inseridas; i++){
            novo_irmao->chaves_inseridas[i] = no_cheio->chaves_inseridas[i + indice_mediana];
        }
        
        no_cheio->quantidade_chaves_inseridas = indice_mediana;
        novo_irmao->proximo_fila = no_cheio->proximo_fila;
        no_cheio->proximo_fila = novo_irmao;

        printf("  Conteudo DEPOIS da divisao:\n");
        printf("    No antigo (esq): ");
        for(int k=0; k < no_cheio->quantidade_chaves_inseridas; k++) {
            printf("%s ", no_cheio->chaves_inseridas[k].chave_hash);
        }
        printf("\n    No novo (dir): ");
        for(int k=0; k < novo_irmao->quantidade_chaves_inseridas; k++) {
            printf("%s ", novo_irmao->chaves_inseridas[k].chave_hash);
        }
        printf("\n-----------------------\n");
    }

    //esse bloco é um tratamento geral dos nós pais, não é influenciado se o nó dividido for folha ou não
    for(int i = pai->quantidade_chaves_inseridas; i > indice_no_pai; i--){
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
    no_BplusTree* arvore = NULL;
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
            char palavra2[33];
            if (sscanf(linha, "%*s %32s", palavra2) == 1){

                if (strcmp(palavra2, "RANGE") == 0){
                    arquivo arquivo_temp1, arquivo_temp2;
                    if (sscanf(linha, "%*s %*s %32s %32s", arquivo_temp1.chave_hash, arquivo_temp2.chave_hash) == 2) {
                        lista* lista = busca_intervalo(arvore, arquivo_temp1, arquivo_temp2, grau_maximo_arvore, output);

                        if(lista != NULL && lista->quantidade_nos > 0){
                            fprintf(output, "[%s,%s]\n", arquivo_temp1.chave_hash, arquivo_temp2.chave_hash);
                            Elemento_lista* no_atual = lista->inicio;

                            while(no_atual != NULL){
                                for(int j = 0; j < no_atual->no_armazenado->quantidade_chaves_inseridas; j++){
                                    fprintf(output, "%s:size=%d,hash=%s\n", no_atual->no_armazenado->chaves_inseridas[j].nome, 
                                    no_atual->no_armazenado->chaves_inseridas[j].tamanho,
                                    no_atual->no_armazenado->chaves_inseridas[j].chave_hash);
                                }

                                no_atual = no_atual->proximo;
                            }
                        }       
                        else{
                            fprintf(output, "[%s,%s]\n", arquivo_temp1.chave_hash, arquivo_temp2.chave_hash);
                            fprintf(output, "-\n");
                        }
                        liberar_lista(lista);
                    }        
                }

                else{
                    arquivo arquivo_temp;
                    if (sscanf(linha, "%*s %32s", arquivo_temp.chave_hash) == 1) {
                        no_BplusTree* no_encontrado = busca(arvore, arquivo_temp, output);

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
        }
    }  
    
    fclose(input);
    fclose(output);
    destruir_arvore(arvore);
    return 0;
}