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
//- CONCERTAR MENSAGEM DE INFECÇÃO

//* Leitura funcionando com liberação de memória OK
//* tratadores de sinal OK
//* Rodar mais de um comando da RUIM [CORRIGIDO]
//* Rodando múltiplos comandos (Mas sem redirecionamento da saída, e sem tratamento de "Virus")
//?  |-> Tá bugando a saída pois não tem o redirecionamento com PIPE
//& Fazer para vários filhos (PIPE!)
//& Implementar armagedon e libera moita
//& Testar sinais no programa todo

//^ O comando virus1 infecta o vsh com SIGUSR1
//^ O comando virus2 infecta o vsh com SIGUSR2

//Tratadores de sinais
void trataSIGINT(){
	printf("Nao funciona fds\n");
}

void trataSIGQUIT(){
	printf("Nao funciona fds\n");
}

void trataSIGTSTP(){
	printf("Nao funciona fds\n");
}

void fiqueiDoente(){
	printf("I feel so sick, goodbye world...\n");
	raise(SIGTERM);
}

void trataSIGUSER1(){
	FILE* f = fopen("shellImune.txt", "r");
	if(f == NULL){
		printf("I feel sick but, I'm immune\n");
		return;
	}
	char c;
	while (!feof(f)){
		fscanf(f, "%c", &c);
		printf("%c", c);
	}
	fclose(f);
}

void trataSIGUSER2(){
	FILE* f = fopen("shellImune.txt", "r");
	if(f == NULL){
		printf("I feel sick but, I'm immune\n");
		return;
	}
	char c;
	while (!feof(f)){
		fscanf(f, "%c", &c);
		printf("%c", c);
	}
	fclose(f);
}

char** linhaDecomando(int* indice){

	char linha[MAX_STRING];
	char* result;
	char** comandos = (char**)malloc(sizeof(char*)*MAX_COMANDOS);
	char* token = "A";

	result = fgets(linha, MAX_STRING, stdin);

	//Faz a leitura de uma linha inteira de qualquer tamanho
	if (result == NULL){
		printf("Ocorreu um erro na leitura\n");
		return NULL;
	}
	if(result[0] == 10){
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
	signal(SIGUSR1, trataSIGUSER1);

	printf("vsh> ");
	
	int cont = 0;
	//& CORPO DO WHILE
	while(cont < 10){

		indice = 0;
		comandos = linhaDecomando(&indice);
		if(comandos == NULL){
			cont++;
			continue;
		}
	
		int c_pid[indice];//Armazenar o pid de todos os filhos

		//-DEBUG PARA TESTAR SINAIS
		if(strcmp(comandos[0], "virus1") == 0){
			raise(SIGUSR1);
		}else if(strcmp(comandos[0], "virus2") == 0){
			raise(SIGUSR2);
		}else if(indice == 1){//foreground

			if((c_pid[0] = fork()) < 0){
				printf("Infelizmente um erro ocorreu. Falha na criacao de um proceso\n");
				exit(1);
			}

			if(c_pid[0] == 0){
				
				char* flags[10];//Armazena o comando e as flags no formato do exec
				
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
				//Em caso de sucesso o código abaixo não é executado, 
				//caso haja falha, o código abaixo exibe uma mensagem de erro no terminal

				printf("Falha no comando: ");
				for(int k = 0; k < i; k++){
					printf("%s ", flags[k]);
				}
				printf("\n");

				return 0;
			}
			//Aqui o vsh espera pelo término de seu único filho, para simular o foreground
			wait(NULL);
		}else{//background
			for(int p = 0; p < indice; p++){
				
				if((c_pid[p] = fork()) < 0){
					printf("Infelizmente um erro ocorreu. Falha na criacao de um preocesso.\n");
					exit(1);
				}else if(c_pid[p] == 0){//Código do processo Filho
					char* flags[10];
					
					char* token = strtok(comandos[p], " ");
					int i = 0;

					while (token != NULL){
					//	printf("[]%s\n", token);
						flags[i] = token;
						token = strtok(NULL, " ");
						i++;
					}
					flags[i] = NULL;

					execvp(flags[0], flags);
					//Em caso de sucesso o código abaixo não é executado, 
					//caso haja falha, o código abaixo exibe uma mensagem de erro no terminal

					printf("Falha no comando: ");
					for(int k = 0; k < i; k++){
						printf("%s ", flags[k]);
					}
					printf("\n");
					return 0;
				}
			}

			for(int i = 0; i < indice; i++){
				waitpid(c_pid[i], NULL, WNOHANG);
			}
			
		}
		/*Processo Principal*/
		liberaComandos(comandos, indice);
		printf("vsh> ");
		cont++;
	}

	//& -----------------
	
	// //- DEBUG
	// for(int i = 0 ; i < indice; i++){
	// 	printf("[%s]\n", comandos[i]);
	// }
	return 0;
}