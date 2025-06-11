#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

uint8_t checksum(const char *requisicoes){ //o const garante que a string não será alterada, apenas lida
    uint8_t checksum = 0;
        while(*requisicoes){
            checksum ^= (uint8_t)(*requisicoes); //aqui faz a operação xor, o uint8_t já serve como um conversor para tabela ASCII
            requisicoes++; //aqui faz o ponteiro do caractere da string avançar para o próximo caracter até a string acabar
        }
    return checksum; //retorna o valor final após os xors
}

uint32_t checar_servidor(uint32_t servidor[], uint32_t tamanho_servidor){
    for (uint32_t i = 0; i < tamanho_servidor; i++) {
        if (servidor[i] == 0) {
            return 0; //ainda tem espaço
        }
    }
    return 1; //servidor cheio
}


uint32_t hashmap1(uint32_t qtd_servidores, const char *requisicao){
    return (7919 * checksum(requisicao)) % qtd_servidores;
}

uint32_t hashmap2(uint32_t qtd_servidores, const char *requisicao){
    return ((104729 * checksum(requisicao)) + 123) % qtd_servidores;
}

uint32_t double_hashing(uint32_t qtd_servidores, uint32_t servidor[], uint32_t tamanho_servidor, const char *requisicao){
    uint32_t valor = 0;
        for(uint32_t i = 0; i < qtd_servidores; i++){
            valor = (hashmap1(qtd_servidores, requisicao) + i*hashmap2(qtd_servidores, requisicao)) % qtd_servidores;
            if(!checar_servidor(servidor[valor], tamanho_servidor)) return valor; //verifica se o servidor tá livre e aloca valor
        }
    return -1; //tudo cheio
}

void amostrar_servidor(uint32_t servidor[], uint32_t tamanho_servidor, const char *requisicao, FILE* output){
    for(int i = 0; i < tamanho_servidor; i++){
        
    }
}


int main(uint32_t argc, char* argv[]){
    char requisicao[101];
    uint32_t qtd_requisicoes, **servidor, qtd_servidores, capacidade_servidor;
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

    fscanf(input, "%u %u", &qtd_servidores, &capacidade_servidor);

    servidor = (uint32_t*) malloc(qtd_servidores * sizeof(uint32_t));
        for(int i = 0; i < qtd_servidores; i++){
            servidor[i] = (uint32_t*) malloc(capacidade_servidor * sizeof(uint32_t));
        }
    
    fscanf(input, "%u", &qtd_requisicoes);
    
    


    return 0;
}