#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

int main(int argc, char* argv[]){

	if(argc == 1){
		return 0;
	}
	raise(SIGTSTP);
	printf("Hello %s\n", argv[1]);

	return 0;
}