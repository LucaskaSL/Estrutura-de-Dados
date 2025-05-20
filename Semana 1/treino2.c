#include <stdio.h>
#include <string.h>
#include <conio.h>
#include <stdlib.h>

//gcc treino2.c -o treino2

void saudacao() {
    printf("Ola, tudo bem?\n");
}

void despedida() {
    printf("Ate logo!\n");
}

int main(int argc, char *argv[]) {
if (argc < 3) {
    printf("Uso correto: %s <num1> <num2>\n", argv[0]);
    return 1;
}
int num1, num2, resultado;
printf("\nMultiplicando valores passados na linha de comando\n");
num1 = atoi(argv[1]); //atoi converte alfanumericos em numeros decimais
num2 = atoi(argv[2]);
resultado = num1 * num2;
  printf("%d X %d = %d\n",num1 ,num2, resultado);
  //getch();


   if (argc < 4) {
        printf("Uso: %s [saudacao|despedida]\n", argv[0]);
        return 1;
    }

    if (strcmp(argv[3], "saudacao") == 0) {
        saudacao();
    } else if (strcmp(argv[3], "despedida") == 0) {
        despedida();
    } else {
        printf("Comando desconhecido: %s\n", argv[3]);
    }

    return 0;
}
