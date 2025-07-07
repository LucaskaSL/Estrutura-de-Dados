#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>

//gcc Biblioteca.c -o Biblioteca
//./Biblioteca input.txt output.txt

typedef struct livros{
    uint64_t codigo_ISBN;
    char titulo_livro[100], nome_autor[50];
} livros;

void trocar(livros* a, livros* b) {
    livros temp = *a;
    *a = *b;
    *b = temp;
}

int particionar(livros* arr, int baixo, int alto) {
    livros pivo = arr[alto];
    int i = baixo - 1;

    for (int j = baixo; j < alto; j++) {
        if (arr[j].codigo_ISBN < pivo.codigo_ISBN || (arr[j].codigo_ISBN == pivo.codigo_ISBN)) {
            i++;
            trocar(&arr[i], &arr[j]);
        }
    }

    trocar(&arr[i + 1], &arr[alto]);
    return i + 1;
}

void quicksort(livros* arr, int baixo, int alto) {
    if (baixo < alto) {
        int pi = particionar(arr, baixo, alto);
        quicksort(arr, baixo, pi - 1);
        quicksort(arr, pi + 1, alto);
    }
}

int64_t busca_binaria(livros* vetor, uint32_t quantidade_livros, uint64_t codigo_ISBN_livro_procurado, uint32_t* passos_realizados){
    int32_t i = 0, j = quantidade_livros - 1;
    int64_t pivo = (i+j)/2;
    *passos_realizados = 0;

    while(j >= i && vetor[pivo].codigo_ISBN != codigo_ISBN_livro_procurado){
        pivo = (i+j)/2;
        (*passos_realizados)++; 
        
        if (vetor[pivo].codigo_ISBN == codigo_ISBN_livro_procurado) {
            return pivo;
        }

        else if(vetor[pivo].codigo_ISBN > codigo_ISBN_livro_procurado){
            j = pivo - 1;
        } 

        else{
            i = pivo + 1;
        }
    }
    (*passos_realizados)++; 
    return -1;
}

int64_t busca_interpolada(livros* vetor, uint32_t quantidade_livros, uint64_t codigo_ISBN_livro_procurado, uint32_t* passos_realizados) {
    int32_t i = 0, j = quantidade_livros - 1;
    *passos_realizados = 0;
    int64_t pivo = i + (int64_t)(vetor[j].codigo_ISBN - vetor[i].codigo_ISBN) % (j - i + 1);

    while (j >= i && vetor[pivo].codigo_ISBN != codigo_ISBN_livro_procurado) {
        (*passos_realizados)++; 
        int64_t pivo = i + (int64_t)(vetor[j].codigo_ISBN - vetor[i].codigo_ISBN) % (j - i + 1);

        if (vetor[pivo].codigo_ISBN == codigo_ISBN_livro_procurado) {
            return pivo;
        } 
        
        else if (vetor[pivo].codigo_ISBN > codigo_ISBN_livro_procurado) {
            j = pivo - 1;
        }
        
        else {
            i = pivo + 1;
        }
    }
    return -1;
}

void modificar_nome(char* texto){
for(int i = 0; (texto[i] != '\0'); i++){
    if(texto[i] == ' '){
        texto[i] = '_';
}
}
}

void amostrar_tudo(livros* vetor, uint32_t quantidade_livros, uint64_t* buscas, uint32_t quantidade_buscas, FILE* output){
    uint32_t passos_realizados_bb, passos_realizados_bi, vitorias_bb = 0, vitorias_bi = 0;
    uint32_t passos_realizados_bb_total = 0, passos_realizados_bi_total = 0;

    for(int i = 0; i < quantidade_buscas; i++){
    int64_t pos_bb = busca_binaria(vetor, quantidade_livros, buscas[i], &passos_realizados_bb);
    (void)busca_interpolada(vetor, quantidade_livros, buscas[i], &passos_realizados_bi);

    passos_realizados_bb_total += passos_realizados_bb;
    passos_realizados_bi_total += passos_realizados_bi;

    if(pos_bb != -1){
        //char autor_temp[51], titulo_temp[101];
        //strcpy(autor_temp, vetor[pos_bb].nome_autor);
        //strcpy(titulo_temp, vetor[pos_bb].titulo_livro);
        modificar_nome(vetor[pos_bb].nome_autor);
        modificar_nome(vetor[pos_bb].titulo_livro);

        fprintf(output, "[%" PRIu64 "]B=%u|I=%u|Author:%s,Title:%s\n", buscas[i], passos_realizados_bb, passos_realizados_bi, vetor[pos_bb].nome_autor, vetor[pos_bb].titulo_livro);
    }
    else{
        fprintf(output, "[%" PRIu64 "]B=%u|I=%u|ISBN_NOT_FOUND\n", buscas[i], passos_realizados_bb, passos_realizados_bi);
    }

    if(passos_realizados_bi <= passos_realizados_bb){
        vitorias_bi++;
        }
    else{
        vitorias_bb++;
    }
}
    fprintf(output, "Binary=%u:%u\n", vitorias_bb, passos_realizados_bb_total/quantidade_buscas);
    fprintf(output, "Interpolation=%u:%u", vitorias_bi, passos_realizados_bi_total/quantidade_buscas);
}

int main(int argc, char* argv[]){
    FILE* input = fopen(argv[1], "r");
    if(input == NULL){
        printf("Erro ao abrir o arquivo de input");
        return 1;
}

    FILE* output = fopen(argv[2], "w");
    if(output == NULL){
        printf("Erro ao abrir o arquivo de output");
        return 1;
}

    uint32_t quantidade_livros;
    fscanf(input, "%u", &quantidade_livros);
    livros *vetor = (livros*) malloc(quantidade_livros * sizeof(livros));
    for(int i = 0; i < quantidade_livros; i++){
        fscanf(input, "%" SCNu64 " %[^&]&%[^\n]", &vetor[i].codigo_ISBN, vetor[i].nome_autor, vetor[i].titulo_livro);
    }

    //quicksort(vetor, 0, quantidade_livros - 1);

    uint32_t quantidade_buscas;
    fscanf(input, "%u", &quantidade_buscas);
    uint64_t *buscas = (uint64_t*) malloc(quantidade_buscas * sizeof(uint64_t));
    for(int i = 0; i < quantidade_buscas; i++){
        fscanf(input, "%" SCNu64, &buscas[i]);
    }


    amostrar_tudo(vetor, quantidade_livros, buscas, quantidade_buscas, output);
    


    free(vetor);
    free(buscas);
    fclose(input);
    fclose(output);
    return 0;
}