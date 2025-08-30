#include <stdio.h>
#include <inttypes.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

//gcc -Wall lucassantanaleal_202400028946_controledesenha.c -o lucassantanaleal_202400028946_controledesenha
//./lucassantanaleal_202400028946_controledesenha input.txt output.txt

typedef struct Paciente{
    struct Paciente* proximo;
    char nome[51];
    uint32_t idade;
} Paciente;

typedef struct Fila {
    Paciente* inicio;
    Paciente* fim;
} Fila;

typedef struct Orgao{
    char nome[51];
    uint32_t quantidade_atendentes;
    Fila fila_preferencial;
    Fila fila_convencional;
} Orgao;

void inicializar_fila(Fila* f) {
    f->inicio = NULL;
    f->fim = NULL;
}

int fila_vazia(Fila* f) {
    return f->inicio == NULL;
}

void enfileirar(Fila* f, const char* nome, int idade) {
    Paciente* novo_paciente = (Paciente*)malloc(sizeof(Paciente));
    strcpy(novo_paciente->nome, nome);
    novo_paciente->idade = idade;
    novo_paciente->proximo = NULL;

    if (fila_vazia(f)) {
        f->inicio = novo_paciente;
        f->fim = novo_paciente;
    } else {
        f->fim->proximo = novo_paciente;
        f->fim = novo_paciente;
    }
}

Paciente* desenfileirar(Fila* f) {
    if (fila_vazia(f)) {
        return NULL;
    }
    Paciente* temp = f->inicio;
    f->inicio = f->inicio->proximo;
    if (f->inicio == NULL) {
        f->fim = NULL;
    }
    return temp;
}

void formatar_nome(char* nome) {
    for (int i = 0; nome[i] != '\0'; i++) {
        if (nome[i] == ' ') {
            nome[i] = '_';
        }
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

    uint32_t quantidade_orgaos;
    fscanf(input, "%u", &quantidade_orgaos);
    Orgao* vetor_de_orgaos = (Orgao*) malloc(sizeof(Orgao) * quantidade_orgaos);

    for(int i = 0; i < quantidade_orgaos; i++){
        fscanf(input, "%s %u", vetor_de_orgaos[i].nome, &vetor_de_orgaos[i].quantidade_atendentes);
        inicializar_fila(&vetor_de_orgaos[i].fila_preferencial);
        inicializar_fila(&vetor_de_orgaos[i].fila_convencional);
    }

    uint32_t quantidade_pacientes;
    fscanf(input, "%u", &quantidade_pacientes);

    for(int i = 0; i < quantidade_pacientes; i++){
        char nome_orgao_temp[51];
        char nome_paciente_temp[51];
        int idade_temp;
        fscanf(input, " %[^|]|%[^|]|%d", nome_orgao_temp, nome_paciente_temp, &idade_temp);
        for (int j = 0; j < quantidade_orgaos; j++) {
            if (strcmp(nome_orgao_temp, vetor_de_orgaos[j].nome) == 0) {
                if (idade_temp >= 60) {
                    enfileirar(&vetor_de_orgaos[j].fila_preferencial, nome_paciente_temp, idade_temp);
                } else {
                    enfileirar(&vetor_de_orgaos[j].fila_convencional, nome_paciente_temp, idade_temp);
                }
                break;
            }
        }
    }

    while (quantidade_pacientes > 0) {
        for (int i = 0; i < quantidade_orgaos; i++) {
            int atendentes_disponiveis = vetor_de_orgaos[i].quantidade_atendentes;
            Paciente* chamados_nesta_rodada[10];
            int num_chamados = 0;

            // Atender preferenciais primeiro
            while (atendentes_disponiveis > 0 && !fila_vazia(&vetor_de_orgaos[i].fila_preferencial)) {
                chamados_nesta_rodada[num_chamados++] = desenfileirar(&vetor_de_orgaos[i].fila_preferencial);
                atendentes_disponiveis--;
                quantidade_pacientes--;
            }

            // Atender convencionais se ainda houver atendentes
            while (atendentes_disponiveis > 0 && !fila_vazia(&vetor_de_orgaos[i].fila_convencional)) {
                chamados_nesta_rodada[num_chamados++] = desenfileirar(&vetor_de_orgaos[i].fila_convencional);
                atendentes_disponiveis--;
                quantidade_pacientes--;
            }

            // Imprimir o resultado da rodada para este órgão
            if (num_chamados > 0) {
                fprintf(output, "%s:", vetor_de_orgaos[i].nome);
                for (int j = 0; j < num_chamados; j++) {
                    formatar_nome(chamados_nesta_rodada[j]->nome);
                    fprintf(output, "%s", chamados_nesta_rodada[j]->nome);
                    if (j < num_chamados - 1) {
                        fprintf(output, ",");
                    }
                    free(chamados_nesta_rodada[j]); // Liberar memória do paciente atendido
                }
                fprintf(output, "\n");
            }
        }
    }

    free(vetor_de_orgaos);



    return 0;
}