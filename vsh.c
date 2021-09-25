/*vaccine shell*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#define MAX_COMANDOS 6
#define MAX_STRING 100
#define MAX_SENT 5
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
//* Rodando múltiplos comandos (Mas sem redirecionamento da saída)
//?  |-> Tá bugando a saída pois não tem o redirecionamento com PIPE
//* Processos em BG terminam quando um dos "irmãos" recebe SIGUSR OK
//& Fazer para vários filhos (PIPE!)
//& Implementar armagedon e libera moita
//& Testar sinais no programa


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
	int pid;//pid do filho em FG
	int sentinela[MAX_SENT];//Sentinelas responsáveis pelos processos BG
	int n = 0;//Número de Sentinelas

    //& CORPO DO WHILE
    while(cont < 10){

        indice = 0;
        comandos = linhaDecomando(&indice);
        if(comandos == NULL){
            cont++;
            continue;
        }

        //-DEBUG PARA TESTAR SINAIS
        if(strcmp(comandos[0], "virus1") == 0){
            raise(SIGUSR1);
        }else if(strcmp(comandos[0], "virus2") == 0){
            raise(SIGUSR2);
        }else if(indice == 1){//foreground

            if((pid = fork()) < 0){
                printf("Infelizmente um erro ocorreu. Falha na criacao de um proceso\n");
                exit(1);
            }

            if(pid == 0){

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

                //sleep(300);

                printf("Falha no comando: ");
                for(int k = 0; k < i; k++){
                    printf("%s ", flags[k]);
                }
                printf("\n");

                return 0;
            }
            //Aqui o vsh espera pelo término de seu único filho, para simular o foreground
			waitpid(pid, NULL, 0);
        }else{//background
			//Coleta de "zombies" (sentinelas que morreram e não tiveram status reportado ao vsh)
			if(n == MAX_SENT){

			}
			if((sentinela[n] = fork()) < 0){
				printf("Infelizmente um erro ocorreu. Falha na criacao de um preocesso.\n");
				exit(1);
			}
			
			if(sentinela[n] == 0){//Código do sentinela (VSH não executa essa parte)
				int c_pid[indice];//Armazenar o pid de todos os filhos
				//setsid();//? Fazer mais testes quando o pipe estiver pronto
				for(int p = 0; p < indice; p++){
					if((c_pid[p] = fork()) < 0){
						printf("Infelizmente um erro ocorreu. Falha na criacao de um preocesso.\n");
						exit(1);
					}else if(c_pid[p] == 0){//Código do processo Filho

						if (p==0){//primeiro filho definido o pgid o proprio pid
						    setpgrp();
						}
						else{//proximos filhos herdam o pgid do primeiro filho
						    setpgid(getpid(), c_pid[0]);
						}

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
					//Para impedir que a race-condition dê problrmas, os grupos são atualizados também na sentinela
					setpgid(c_pid[p], c_pid[0]);
				}
				int status;
				int pid;
				//Espera por todos os filhos
				for(int i = 0; i < indice; i++){
					pid = waitpid(-1*c_pid[0], &status, 0);//Ver se qualquer filho do grupo terminou
					if(pid != -1){
						if(WIFSIGNALED(status)){
							if(WTERMSIG(status) == SIGUSR1){//Se algum filho terminoi de SIGUr
								
								printf("Filho terminou de sigUsr1, terminar os irmaos\n");
								killpg(c_pid[0], SIGTERM);//Envia o sinal de terminação para o grupo todo
								break;
							}
						}
					}
				}
				return 0;
			}else{//vsh
				n++;//incrementa número de sentinelas
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