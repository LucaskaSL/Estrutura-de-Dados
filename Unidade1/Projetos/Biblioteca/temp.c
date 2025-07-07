#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <time.h> // Necessário para srand() e time()

// Para compilar com otimizações (recomendado para desempenho):
// gcc Biblioteca.c -o Biblioteca -O3
// Para executar:
// ./Biblioteca input.txt output.txt

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
    // Escolhe um índice de pivô aleatório dentro do intervalo [baixo, alto]
    int random_pivot_index = baixo + rand() % (alto - baixo + 1);

    // Troca o elemento do pivô aleatório com o elemento final
    // Assim, o restante da lógica de partição (que usa arr[alto] como pivô) funciona sem alterações.
    trocar(&arr[random_pivot_index], &arr[alto]);

    livros pivo = arr[alto]; // Agora, o pivô é o elemento que estava na posição aleatória
    int i = baixo - 1;

    for (int j = baixo; j < alto; j++) {
        // A condição foi simplificada para apenas <, o que é comum.
        // O caso de igualdade (== pivo.codigo_ISBN) é tratado naturalmente na iteração seguinte,
        // ou pelo elemento se tornando o pivô e sendo colocado na posição final.
        if (arr[j].codigo_ISBN < pivo.codigo_ISBN) {
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

// **Busca Binária Refinada para contagem de passos e cálculo de pivô**
int64_t busca_binaria(livros* vetor, uint32_t quantidade_livros, uint64_t codigo_ISBN_livro_procurado, uint32_t* passos_realizados){
    int32_t i = 0, j = quantidade_livros - 1;
    int64_t pivo; // Declara pivo uma vez
    *passos_realizados = 0; // Inicializa passos para 0

    while(j >= i){ // Loop enquanto houver um intervalo de busca válido
        (*passos_realizados)++; // Conta esta iteração/comparação

        pivo = i + (j - i) / 2; // Calcula o pivô para o intervalo atual (previne overflow)

        if (vetor[pivo].codigo_ISBN == codigo_ISBN_livro_procurado) {
            return pivo; // Encontrado
        } else if(vetor[pivo].codigo_ISBN > codigo_ISBN_livro_procurado){
            j = pivo - 1; // Ajusta limite superior
        } else { // vetor[pivo].codigo_ISBN < codigo_ISBN_livro_procurado
            i = pivo + 1; // Ajusta limite inferior
        }
    }
    (*passos_realizados)++; 
    return -1; // Não encontrado
}

// **Busca Interpolada com a SUA nova fórmula do pivô e as condições de loop CORRETAS**
int64_t busca_interpolada(livros* vetor, uint32_t quantidade_livros, uint64_t codigo_ISBN_livro_procurado, uint32_t* passos_realizados) {
    int32_t i = 0, j = quantidade_livros - 1;
    *passos_realizados = 0;
    int64_t pivo = i + (int64_t)(vetor[j].codigo_ISBN - vetor[i].codigo_ISBN) % (j - i + 1);

    while (j >= i && vetor[pivo].codigo_ISBN != codigo_ISBN_livro_procurado) {
        (*passos_realizados)++; // Conta o passo atual (sonda)

        int64_t pivo = i + (int64_t)(vetor[j].codigo_ISBN - vetor[i].codigo_ISBN) % (j - i + 1);

        if (vetor[pivo].codigo_ISBN == codigo_ISBN_livro_procurado) {
            // printf("  [DEBUG] ISBN encontrado no índice %" PRId64 "\n", pivo);
            return pivo; // Encontrado!
        }
        else if (vetor[pivo].codigo_ISBN > codigo_ISBN_livro_procurado) {
            j = pivo - 1; // Ajusta o limite superior
        }
        else { // vetor[pivo].codigo_ISBN < codigo_ISBN_livro_procurado
            i = pivo + 1; // Ajusta o limite inferior
        }
    }

    return -1; // Não encontrado
}

void modificar_nome(char* texto){
    for(int i = 0; (texto[i] != '\0'); i++){
        if(texto[i] == ' '){
            texto[i] = '_';
        }
    }
}

void amostrar_tudo(livros* vetor, uint32_t quantidade_livros, uint64_t* buscas, uint32_t quantidade_buscas, FILE* output){
    uint32_t passos_realizados_bb, passos_realizados_bi, passos_realizados_bb_total = 0, passos_realizados_bi_total = 0;
    uint32_t vitorias_bb = 0, vitorias_bi = 0;

    for(int i = 0; i < quantidade_buscas; i++){
        int64_t pos_bb = busca_binaria(vetor, quantidade_livros, buscas[i], &passos_realizados_bb);
        (void)busca_interpolada(vetor, quantidade_livros, buscas[i], &passos_realizados_bi);

        passos_realizados_bb_total += passos_realizados_bb;
        passos_realizados_bi_total += passos_realizados_bi;

        // Crie cópias das strings para modificação, para não alterar os dados originais no array 'vetor'
        char autor_display[51];
        char titulo_display[101];

        if(pos_bb != -1){ // Se a busca binária encontrou (o que implica que o ISBN existe no vetor)
            strcpy(autor_display, vetor[pos_bb].nome_autor);
            strcpy(titulo_display, vetor[pos_bb].titulo_livro);
            modificar_nome(autor_display);
            modificar_nome(titulo_display);
            fprintf(output, "[%" PRIu64 "]B=%u|I=%u|Author:%s,Title:%s\n", buscas[i], passos_realizados_bb, passos_realizados_bi, autor_display, titulo_display);
        } else {
            fprintf(output, "[%" PRIu64 "]B=%u|I=%u|ISBN_NOT_FOUND\n", buscas[i], passos_realizados_bb, passos_realizados_bi);
        }

        // Lógica de vitória (BI vence se <= BB, caso contrário BB vence)
        if(passos_realizados_bi <= passos_realizados_bb){
            vitorias_bi++;
        } else {
            vitorias_bb++;
        }
    }

    // O cálculo da média DEVE usar double para evitar truncamento de inteiros.
    // A formatação %.0f imprime o resultado como um inteiro arredondado.
    fprintf(output, "Binary=%u:%u\n", vitorias_bb, passos_realizados_bb_total / quantidade_buscas);
    fprintf(output, "Interpolation=%u:%u", vitorias_bi, passos_realizados_bi_total / quantidade_buscas);
}

int main(int argc, char* argv[]){
    // Inicializa o gerador de números aleatórios UMA ÚNICA VEZ no início do programa.
    srand(time(NULL)); 

    if (argc != 3) {
        printf("Uso: %s <arquivo_entrada> <arquivo_saida>\n", argv[0]);
        return 1;
    }

    FILE* input = fopen(argv[1], "r");
    if(input == NULL){
        printf("Erro ao abrir o arquivo de input\n"); // Adicionado newline para saída mais limpa
        return 1;
    }
    // Otimização de I/O: Use um buffer grande para leitura do arquivo
    setvbuf(input, NULL, _IOFBF, 1024 * 1024); // 1MB buffer

    FILE* output = fopen(argv[2], "w");
    if(output == NULL){
        printf("Erro ao abrir o arquivo de output\n"); // Adicionado newline
        fclose(input); // Fechar o arquivo de entrada se o de saída falhar
        return 1;
    }
    // Otimização de I/O: Use um buffer grande para escrita no arquivo
    setvbuf(output, NULL, _IOFBF, 1024 * 1024); // 1MB buffer

    uint32_t quantidade_livros;
    // Adicionar verificação de erro para fscanf
    if (fscanf(input, "%u", &quantidade_livros) != 1) {
        printf("Erro ao ler a quantidade de livros do input\n");
        fclose(input);
        fclose(output);
        return 1;
    }

    livros *vetor = (livros*) malloc(quantidade_livros * sizeof(livros));
    if (vetor == NULL) {
        printf("Erro de alocacao de memoria para livros\n");
        fclose(input);
        fclose(output);
        return 1;
    }

    for(int i = 0; i < quantidade_livros; i++){
        // %*c consome o caractere de nova linha após o último campo lido na linha anterior.
        // Limites de largura de campo (%49[^&], %99[^\n]) para prevenir estouros de buffer.
        if (fscanf(input, "%" SCNu64 " %49[^&]&%99[^\n]%*c", &vetor[i].codigo_ISBN, vetor[i].nome_autor, vetor[i].titulo_livro) != 3) {
            printf("Erro ao ler dados do livro %d. Verifique o formato do input.txt\n", i + 1);
            free(vetor);
            fclose(input);
            fclose(output);
            return 1;
        }
    }

    quicksort(vetor, 0, quantidade_livros - 1);

    uint32_t quantidade_buscas;
    if (fscanf(input, "%u", &quantidade_buscas) != 1) {
        printf("Erro ao ler a quantidade de buscas do input\n");
        free(vetor);
        fclose(input);
        fclose(output);
        return 1;
    }

    uint64_t *buscas = (uint64_t*) malloc(quantidade_buscas * sizeof(uint64_t));
    if (buscas == NULL) {
        printf("Erro de alocacao de memoria para buscas\n");
        free(vetor);
        fclose(input);
        fclose(output);
        return 1;
    }

    for(int i = 0; i < quantidade_buscas; i++){
        if (fscanf(input, "%" SCNu64, &buscas[i]) != 1) {
            printf("Erro ao ler ISBN para busca %d\n", i + 1);
            free(vetor);
            free(buscas);
            fclose(input);
            fclose(output);
            return 1;
        }
    }

    amostrar_tudo(vetor, quantidade_livros, buscas, quantidade_buscas, output);
    
    free(vetor);
    free(buscas);
    fclose(input);
    fclose(output);
    return 0;
}