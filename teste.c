#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <time.h>

int main(){

	sleep(60);
	printf("Estou terminando normalmente\n");
	return 0;
}