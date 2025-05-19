#include <stdio.h>
#include <stdint.h>

void funcao_param_valor(uint32_t y){
printf("y = %u\n", y);
y = 18;
printf("y = %u\n", y);
}

void funcao_param_referencia(uint32_t* y){
printf("y = %u\n", *y);
*y = 18; //é como se eu tivesse feito x = 18 na main
printf("y = %u\n", *y);
}

/*void funcao_param_referencia_const(const uint32_t* y){
printf("y = %u\n", *y);
*y = 18; //dá erro logo aqui, porque estou tentando mudar o valor duma constante
printf("y = %u\n", *y);
}*/

int main(int argc, char* argv[]){

char **args = argv;
char ***pargs = &args;

printf("\nmain(%i, %s)\n", argc, args[0]);
printf("main(%i, %s)\n\n", argc, (*pargs)[0]);

uint32_t y = 2;
uint32_t x = 7;
uint32_t* px = NULL; //é o ponteiro que está sendo inicializado apontado para um endereço NULL na memória
//é bom apontar para o NULL porque possui mais controle

px = &x; //aqui ele está recebendo o endereço de memória de x, ou seja, o "valor" do ponteiro passa a ser
//o endereço de x seria, no meu caso, 0x0061FF18
//o endereço real do ponteiro é 4 bytes depois

printf("x = %u @ %p \n", *px, px); //o * significa desreferenciamento de um ponteiro, ou seja,
//ele pega o valor da varíavel que está no endereço de momória que o ponteiro está apontando, *px == x

x = 10;
printf("x = %u\n", *px);
*px = 5;
printf("x = %u\n\n", x);


funcao_param_valor(y);
printf("y = %u\n\n", y);

funcao_param_referencia(&y);
printf("y = %u\n\n", y);



return 0;
}