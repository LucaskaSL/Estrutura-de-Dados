#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//gcc lucassantanaleal_202400028946_engenhodebusca.c -o  lucassantanaleal_202400028946_engenhodebusca
//./lucassantanaleal_202400028946_engenhodebusca input.txt output.txt


uint8_t checksum(const char *requisicoes){ //o const garante que a string não será alterada, apenas lida
    uint8_t checksum = 0;
        while(*requisicoes){
            checksum ^= (uint8_t)(*requisicoes); //aqui faz a operação xor, o uint8_t já serve como um conversor para tabela ASCII
            requisicoes++; //aqui faz o ponteiro do caractere da string avançar para o próximo caracter até a string acabar
        }
    return checksum; //retorna o valor final após os xors
}

char *strdup_custom(const char *src) {
    char *dest = malloc(strlen(src) + 1); // +1 para o '\0'
    if (dest) strcpy(dest, src);
    return dest;
}

uint32_t hashing(uint32_t qtd_servidores, const char *requisicao, uint32_t tentantiva){
    return ((7919 * checksum(requisicao)) + tentantiva*(((104729 * checksum(requisicao)) + 123))) %qtd_servidores;
}

uint32_t inserir_servidor_teste(char ***servidor, uint32_t tamanho_servidor, uint32_t qtd_servidores, char *requisicao, char *requisicao_armazenada, FILE* output){
    uint32_t tentativa = 0;
    while(1){
        uint32_t pos = hashing(qtd_servidores, requisicao, tentativa);

            for(int j = 0; j < tamanho_servidor; j++){
                if(servidor[pos][j] == NULL){
                    servidor[pos][j] = strdup_custom(requisicao_armazenada);

                    if(tentativa > 0){
                        fprintf(output, "S%u->S%u\n", hashing(qtd_servidores, requisicao, 0), pos);
                    }
                return pos;
                }
            }
    tentativa++;
    }
}

void amostrar_servidor(char ***servidor, uint32_t tamanho_servidor, uint32_t pos, FILE* output) {
    fprintf(output, "S%u:", pos);

    int primeira = 1; //só pra ter um rastreio do "tamanho" do servidor

    for (uint32_t i = 0; i < tamanho_servidor; i++) {
        if (servidor[pos][i] != NULL) {
            if (!primeira) { //na primeira iteração do for não acontece nada, nas próximas(se houver elemento) coloca a , antes de printar o próximo
                fprintf(output, ",");
            }
            fprintf(output, "%s", servidor[pos][i]);
            primeira = 0;
        }
    }

    fprintf(output, "\n");
}

void concatenar_sem_underscore(char *destino, char **strings, int qtd){ // Para hash:

    destino[0] = '\0';
    for(int i = 0; i < qtd; i++) {
        strcat(destino, strings[i]);
    }
}

void concatenar_com_underscore(char *destino, char **strings, int qtd){ // Para armazenar/exibir:
    destino[0] = '\0';
    for(int i = 0; i < qtd; i++){
        strcat(destino, strings[i]);
        if(i < qtd - 1)
            strcat(destino, "_");
    }
}


int main(int argc, char* argv[]){
    char ***servidor;
    uint32_t qtd_requisicoes, qtd_servidores, tamanho_servidor;
    FILE* input = fopen(argv[1], "r");
    if(input == NULL){
        perror("Erro ao abrir o arquivo de input");
        return 1;
    }
    FILE* output = fopen(argv[2], "w");
    if(output == NULL){
        perror("Erro ao abrir o arquivo de output");
        return 1;
    }

    fscanf(input, "%u %u", &qtd_servidores, &tamanho_servidor);

    servidor = malloc(qtd_servidores * sizeof(char**));
        for(uint32_t i = 0; i < qtd_servidores; i++){
            servidor[i] = calloc(tamanho_servidor, sizeof(char*)); //cada posição agora é NULL inicialmente
        }
    
    fscanf(input, "%u", &qtd_requisicoes);
    
    uint32_t qtd_strings;
    while (fscanf(input, "%u", &qtd_strings) != EOF) {
        char **nomes = malloc(qtd_strings * sizeof(char*));
        for (uint32_t i = 0; i < qtd_strings; i++) {
            nomes[i] = malloc(101 * sizeof(char)); // até 100 caracteres + \0
            fscanf(input, "%s", nomes[i]);
        }

    // Concatenar para hash
    char nome_para_hash[1001] = ""; // tamanho total maximo
    concatenar_sem_underscore(nome_para_hash, nomes, qtd_strings);

    // Concatenar para armazenar
    char nome_para_armazenar[1001] = "";
    concatenar_com_underscore(nome_para_armazenar, nomes, qtd_strings);

    uint32_t pos_inserido = inserir_servidor_teste(servidor, tamanho_servidor, qtd_servidores, nome_para_hash, nome_para_armazenar, output);
        if (pos_inserido != -1) amostrar_servidor(servidor, tamanho_servidor, pos_inserido, output);

    for (uint32_t i = 0; i < qtd_strings; i++) {
        free(nomes[i]); //libera cada caracter
    }
    free(nomes); //libera a string toda
}

    for (uint32_t i = 0; i < qtd_servidores; i++) {
        for (uint32_t j = 0; j < tamanho_servidor; j++) {
            free(servidor[i][j]); // libera cada string armazenada (se não for NULL)
        }
    free(servidor[i]); // libera o vetor de ponteiros para strings
    }
    free(servidor); // libera o vetor de ponteiros para servidores

    fclose(input);
    fclose(output);
    return 0;
}