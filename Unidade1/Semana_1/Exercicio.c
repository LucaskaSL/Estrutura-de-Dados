#include <stdio.h>
#include <stdint.h>

int main(int argc, char* argv[]){
//Primeira Questão
int x = 2;
    if ((*(char *)&x) == 2) { //aqui eu transformo  o int *, que é o valor ponteiro de &x para char *, e desreferencio
                              //lendo assim o primeiro caracter
        printf("Little Endian\n");
}   else {
        printf("Big Endian\n\n"); //dá esse resultado caso os valores serem iguais e minha máquina seja big endian,
                                //pois na leitura o valor de x ia tá no outro lado
                                //a outra possibilidade é se os valores forem diferentes, até porque cairia no else
}

//Segunda Questão
uint16_t y = 1;
uint16_t* py = NULL;
uint32_t z = 0;
uint32_t* pz = NULL;
float a = 5;
float* pa = NULL;
double b = 10;
double* pb = NULL;
char c = " ";
char* pc = NULL;
py = &y;
pz = &z;
pa = &a;
pb = &b;
pc = &c;
printf("Endereco de y = %p, tamanho: %d\n", py, sizeof(y));
printf("Endereco de z = %p, tamanho: %d\n", pz, sizeof(z));
printf("Endereco de a = %p, tamanho: %d\n", pa, sizeof(a));
printf("Endereco de b = %p, tamanho: %d\n", pb, sizeof(b));
printf("Endereco de c = %p, tamanho: %d\n", pc, sizeof(c));
//o endereço de uma variável pra outra não não diminui igual de acordo com o byte de cada uma, pois ocorre o padding,
//deixando o endereço da variável múltiplo ao valor em bytes dela
//o endereço não diminui por causa do endianness, mas pelo fato da pilha(stack) crescer para baixo, ou seja,
//endereços menores indicam posições mais recentes

//Quarta Questão







return 0;
}