/*vaccine shell*/
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>

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
//* Rodando múltiplos comandos (Mas sem redirecionamento da saída)
//?  |-> Tá bugando a saída pois não tem o redirecionamento com PIPE
//* Processos em BG terminam quando um dos "irmãos" recebe SIGUSR OK
//& Fazer para vários filhos (PIPE!)
//& Implementar armagedon e libera moita
//& Testar sinais no programa


//^ O comando virus1 infecta o vsh com SIGUSR1
//^ O comando virus2 infecta o vsh com SIGUSR2


void trataArmagedon(){
    killpg(0, SIGTERM);
}

void trataSIGUSER(){
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

//funcao que fecha os pipes
void closeAllPipes(int nPipes, int fd[][2]){
    for (int i = 0; i < nPipes; ++i) {
        close(fd[i][0]);
        close(fd[i][1]);
    }
}

int main(){

    char** comandos = NULL;
    int indice = 0;

    //Instalando tratadores de sinais
    signal(SIGUSR1, trataSIGUSER);
    signal(SIGUSR2, trataSIGUSER);

    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGTSTP);
    sigaddset(&mask, SIGQUIT);
    sigaddset(&mask, SIGINT);

    if(sigprocmask(SIG_SETMASK, &mask, NULL)){
        printf("Erro\n");
        return 0;
    }


    int pid;//pid do filho em FG

    //Sentinelas responsáveis pelos processos BG
    int* sentinela = (int*)malloc(sizeof(int)*5);

    int n = 0;//Número de Sentinelas
    int prox = 0;
    int tamSentinelas = 5;//Tamanho do vetor de sentinelas

    for(int i = 0; i < tamSentinelas; i++){
        sentinela[i] = 0;
    }


	// 4 pipes, pois sao 5 processos no maximo, 4 ligacoes entre eles
    int fd[4][2];

    //f[x][0] = read
    //f[x][1] = write

    // filho1 ->  filho2
    //f[0][1] -> f[0][0]

    //filho2  -> filho3
    //f[1][1] -> f[1][0]

    //filho3  -> filho4
    //f[2][1] -> f[2][0]

    //filho4  -> filho5
    //f[3][1] -> f[3][0]

    /*
     * filho1: f[0][1]
     * filho2: f[0][0] | f[1][1]
     * filho3: f[1][0] | f[2][1]
     * filho4: f[2][0] | f[3][1]
     * filho5: f[3][0]
     * */

    printf("vsh> ");
    while(1){

        indice = 0;
        comandos = linhaDecomando(&indice);
        if(comandos == NULL){
            printf("vsh> ");
            continue;
        }

        if(strcmp(comandos[0], "armagedon") == 0){
            printf("Encerrando todas as operacoes\n");
            for(int i = 0; i < tamSentinelas; i++){
                if(sentinela[i] != 0){
                    kill(sentinela[i], SIGUSR1);
                }
            }

            for(int i = 0; i < tamSentinelas; i++){
                if(sentinela[i] != 0){
                    waitpid(sentinela[i], NULL, 0);
                }
            }
            free(sentinela);
            free(comandos);
            raise(SIGTERM);
        }else if(strcmp(comandos[0], "liberamoita") == 0){
            for(int i = 0; i < tamSentinelas; i++){
                if(sentinela[i] != 0){
                    if(waitpid(sentinela[i], NULL, WNOHANG) > 0){//negativo pra erro, 0 para filho n terminou
                        sentinela[i] = 0;
                        n--;
                    }
                }
            }
        }else if(indice == 1){//foreground

            if((pid = fork()) < 0){
                printf("Infelizmente um erro ocorreu. Falha na criacao de um proceso\n");
                exit(1);
            }

            if(pid == 0){

                //Adiciona sinais na máscara do processo FG
                sigaddset(&mask, SIGUSR1);
                sigaddset(&mask, SIGUSR2);
                sigprocmask(SIG_SETMASK, &mask, NULL);

                char* flags[10];//Armazena o comando e as flags no formato do exec

                char* token = strtok(comandos[0], " ");
                int i = 0;

                while (token != NULL){
                    //	printf("[]%s\n", token);
                    flags[i] = token;
                    token = strtok(NULL, " ");
                    i++;
                }

                //ultimo parametro do exec tem q ser NULL
                flags[i] = NULL;

                //flags[0] = ls
                //flags[1] = -l
                //flags[2] = NULL
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
            waitpid(pid, NULL, 0);
            setbuf(stdin, NULL);
        }else{//background

            //Coleta de "zombies" (sentinelas que morreram e não tiveram status reportado ao vsh)
            if(n == tamSentinelas || n == 5){//Procura por filhos zombies

                prox = -1;
                for(int i = 0; i < tamSentinelas; i++){
                    if(waitpid(sentinela[i], NULL, WNOHANG) > 0){
                        sentinela[i] = 0;
                        prox = i;
                        n--;
                    }
                }

                if(prox == -1){//vetor cheio e nenhum dos filhos terminou

                    int newtam = tamSentinelas + 3;

                    sentinela = realloc(sentinela, sizeof(int) * newtam);

                    for(int i = tamSentinelas; i < (tamSentinelas+3); i++){
                        sentinela[i] = 0;
                    }

                    prox = tamSentinelas;
                    tamSentinelas+=3;
                }
            }else{//Procura prox posição vazia
                for(int i = 0; i < tamSentinelas; i++){
                    if(sentinela[i] == 0){
                        prox = i;
                        break;
                    }

                }
            }

            //? -----------------------
            if((sentinela[prox] = fork()) < 0){
                printf("Infelizmente um erro ocorreu. Falha na criacao de um preocesso.\n");
                exit(1);
            }

            if(sentinela[prox] == 0){//Código do sentinela (VSH não executa essa parte)

                sigemptyset(&mask);
                sigprocmask(SIG_SETMASK, &mask, NULL);

                signal(SIGUSR1, trataArmagedon);

                //criacao e verificação dos pipes com base no numero de processos
                int pipes = indice -1;
                for (int i = 0; i < pipes; i++) {
                    if (pipe(fd[i]) == -1){
                        printf("DEU ruim\n");
                        return 1;
                    }
                }

                int c_pid[indice];//Armazenar o pid de todos os filhos
                //setsid();
                setpgrp();
                for(int p = 0; p < indice; p++){
                    if((c_pid[p] = fork()) < 0){
                        printf("Infelizmente um erro ocorreu. Falha na criacao de um processo.\n");
                        exit(1);
                    }else if(c_pid[p] == 0){//Código do processo Filho

                        sigset_t mask2;
                        sigemptyset(&mask);
                        sigprocmask(SIG_SETMASK, &mask2, &mask);//Processos filhos não estão protegidos de nenhum sinal

                        setpgid(c_pid[p], getppid());//Processos filhos no msm grupo do sentinela

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

                        if (p == 0){
                            //primeiro filho
                            //filho1: f[0][1]

                            //faz a saida do exec ser direcionada ao pipe
                            dup2(fd[p][1], STDOUT_FILENO);

                        }else if (p == 4){
                            //quinto filho
                            //filho5: f[3][0]
                            dup2(fd[p - 1][0], STDIN_FILENO);//Nota
                        }
                        else{
                            //segundo, terceiro, quarto filho
                            //filho2: f[0][0] | f[1][1]
                            //filho3: f[1][0] | f[2][1]
                            //filho4: f[2][0] | f[3][1]
                            dup2(fd[p - 1][0], STDIN_FILENO);

                            //verifica se ainda existe um pipe a receber a saida deste processo
                            if (pipes > p){
                                dup2(fd[p][1], STDOUT_FILENO);
                            }
                        }
                        closeAllPipes(pipes, fd);

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
                    setpgid(c_pid[p], getpid());
                }

                closeAllPipes(pipes, fd);

                int status;
                int pid1;

                //Espera por todos os filhos
                for(int i = 0; i < indice; i++){
                    pid1 = waitpid(-1, &status, 0);//Ver se qualquer filho do grupo terminou
                    if(pid1 != -1){
                        if(WIFSIGNALED(status)){
                            if(WTERMSIG(status) == SIGUSR1 || WTERMSIG(status) == SIGUSR2){//Se algum filho terminoi de SIGUr

                                killpg(c_pid[0], SIGTERM);//Envia o sinal de terminação para o grupo todo
                                break;
                            }
                        }
                    }
                }

                return 0;
            }
            else{//vsh
                n++;//incrementa número de sentinelas
            }
        }
        /*Processo Principal*/
        liberaComandos(comandos, indice);
        sleep(1);
        printf("vsh> ");
    }

    return 0;
}