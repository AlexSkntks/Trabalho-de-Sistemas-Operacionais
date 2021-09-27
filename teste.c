#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int* criaVetor(int tam){
	int* vetor = (int*)malloc(sizeof(int)*(tam));

	return vetor;
}

int* aumentaVetor(int* vetor, int* tam){
	int* aux = (int*) malloc(sizeof(int)*(*tam+5));

	for(int i = 0; i < *tam; i++){
		aux[i] = vetor[i];
	}
	free(vetor);
	*tam += 5;
	return aux;
}

int main(){

	int tam = 10;
	int qtd = 0;

	srand(time(NULL));

	int* vetor = criaVetor(tam);
	
	for(int i = 0; i < tam; i++){

		vetor[i] = i;		
		
		qtd++;
	}

	vetor = aumentaVetor(vetor, &tam);

	vetor[qtd++] = 0;
	vetor[qtd++] = 0;
	vetor[qtd++] = 0;

	for(int i = 0; i < qtd; i++){
		printf("%d ", vetor[i]);
	}
	printf("\n");
	return 0;
}