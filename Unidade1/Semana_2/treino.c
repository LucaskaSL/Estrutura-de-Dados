#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void trocar(uint32_t *a, uint32_t *b){
uint32_t temp = *a;
*a = *b;
*b = temp;
}


// Procedimento de ordenação por seleção
void selection_sort(uint32_t* V, uint32_t n) {
for(uint32_t i = 0; i < n - 1; i++) {
uint32_t min = i;
    for(uint32_t j = i + 1; j < n; j++)
    if(V[j] < V[min]) min = j;
    if(i != min) trocar(&V[i], &V[min]);
}
} //terá n^2 passos

void insertion_sort(uint32_t* V, uint32_t n) {
for(uint32_t i = 1; i < n; i++)
for(uint32_t j = i; j > 0 && V[j - 1] > V[j]; j--)
trocar(&V[j], &V[j - 1]);
} //terá n < número de passos < n^2

int main(){
uint32_t *lista, tamanho;
printf("Qual o tamanho da lista que deseja criar? ");
scanf("%u", &tamanho);

lista = (uint32_t*) malloc(tamanho * sizeof(uint32_t));
for(int x = 0; x < tamanho; x++){
    printf("Digite o elemento %u da lista: ", x+1);
    scanf("%u", &lista[x]);
}

selection_sort(lista, tamanho);
for(int x = 0; x < tamanho; x++){
    printf("%u\n", lista[x]);
}

free(lista);
return 0;
}