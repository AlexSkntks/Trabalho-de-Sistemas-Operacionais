/*vaccine shell*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_COMANDOS 6
#define MAX_STRING 100
//SIGUSER1 e SIGUSER2 São fatais
/*
& Além disso, durante a execução do tratador desses sinais, os seguintes sinais que podem ser gerados
& via comando especial “Ctrl-...” ( isto é, Ctrl-C (SIGINT), Ctrl-\ (SIGQUIT), Ctrl-Z (SIGTSTP))
& devem estar bloqueados, para evitar que o tratamento dos sinais SIGUSR1 e SIGUSR2 seja
& interrompido no meio.
*/
/*
- liberamoita: 
faz com que o shell libere todos os seus descendentes (diretos e indiretos,
isto é, filhos, netos, bisnetos, etc.) que estejam no estado “Zombie” antes de
exibir um novo prompt.
*/ 
/*
- armagedon:
termina a operação do shell, mas antes disso, ele deve matar todos os seus
descendentes (diretos e indiretos, isto é, filhos, netos, bisnetos, etc.) que
ainda estejam rodando.
*/

//- LEMBRAR DE TRATAR CTRL-C
//* Leitura funcionando com liberação de memória OK
//* tratadores de sinal OK
//- Rodar mais de um comando da RUIM (BUG)
//& Fazer para vários filhos (PIPE!)
//& Testar sinais no programa todo

//Tratadores de sinais
sig_t trataSIGINT(){
	printf("Nao funciona fds\n");
}

void trataSIGQUIT(){
	printf("Nao funciona fds\n");
}

void trataSIGTSTP(){
	printf("Nao funciona fds\n");
}


char** linhaDecomando(int* indice){

	char* linha;
	char* result;
	char** comandos = (char**)malloc(sizeof(char*)*MAX_COMANDOS);
	char* token = "A";

	result = fgets(linha, MAX_STRING, stdin);

	//Faz a leitura de uma linha inteira de qualquer tamanho
	if (result == NULL){
		printf("Ocorreu um erro na leitura\n");
		return NULL;
	}

	//Separa os comandos passados via shell em strings separadas (máx 5 comandos)
	token = strtok(linha, "|\n");

	while (token != NULL){
		comandos[*indice] = strdup(token);
		token = strtok(NULL, "|\n");
		(*indice)++;
	}

	comandos[*indice] = NULL;//Para usar no formato do execv..
	return comandos;
}

void liberaComandos(char** vString, int indice){
	for(int i = 0; i < indice; i++){
		free(vString[i]);
	}
	free(vString);
}

int main(){

	char** comandos;
	int indice = 0;

	//Instalando tratadores de sinais
	signal(SIGQUIT, trataSIGQUIT);
	signal(SIGTSTP, trataSIGTSTP);

	printf("vsh>: ");

	int cont = 0;
	//& CORPO DO WHILE
	while(cont < 3){
		comandos = linhaDecomando(&indice);
		//parte foreground
		if(indice == 1){

			int pid;
			if((pid = fork()) < 0){
				printf("Infelizmente um erro ocorreu. Falha na criacao de um proceso\n");
				exit(1);
			}

			if(pid == 0){

				char* flags[10];//Armazena o comando e as flags no formato do exec
				//printf("[%s]\n", comandos[0]);
				char* token = strtok(comandos[0], " ");
				int i = 0;

				while (token != NULL){
				//	printf("[]%s\n", token);
					flags[i] = token;
					token = strtok(NULL, " ");
					i++;
				}
				flags[i] = NULL;

				execvp(flags[0], flags);

				printf("Falha no comando: ");
				for(int k = 0; k < i; k++){
					printf("%s ", flags[k]);
				}
				printf("\n");
				return 0;
			}
		}else{
			/*Vários filhos*/
		}
		/*Processo Principal*/
		wait(NULL);
		printf("vsh> ");
		cont++;
	}
	
	
	
	//& -----------------
	
	// //- DEBUG
	// for(int i = 0 ; i < indice; i++){
	// 	printf("[%s]\n", comandos[i]);
	// }

	liberaComandos(comandos, indice);
	return 0;
}