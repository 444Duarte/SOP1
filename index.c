#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <libgen.h>
#include <limits.h>
#include <sys/wait.h>

int main(int argc, char *argv[]) {
	if (argc < 2) { // fails if no argument, optional
		printf("Falta argumento!\n");
		return 0;
	}
	char wordsPath[200];
	char* realPathArg0 = dirname(realpath(argv[0],NULL));
	char* realPathArg1 = realpath(argv[1],NULL);
	sprintf(wordsPath, "%s/words.txt", realPathArg1);
	if(realPathArg1 == NULL)
	{
		printf("A pasta dada no argumento não existe!");
		return 0;
	}
	
	if(fopen(wordsPath,"r") == NULL){ // fails if no words.txt
		printf("Nao existe words.txt!\n");
		return 0;
	}
	int i = 1;
	char str[200];
	sprintf(str, "%s/%d.txt", realPathArg1, i);
	while (open(str, O_RDONLY) >= 0) { // confirm the file i.txt exists
		pid_t pid = fork();
		if (pid == 0) {
			char swCall[PATH_MAX+1];
			sprintf(swCall,"%s/SW",realPathArg0);     
			execlp(swCall,"SW",&i,realPathArg1,NULL);//calling SW
		}
		else if(pid < 0){
			printf("Ocorreu erro ao executar fork!");
			exit(1);
		}
		else{
		int status;
		wait(&status);
		i++;
		sprintf(str, "%s/%d.txt", realPathArg1, i);
		}
	}
	if(i == 1){ // fails if no text files to index
		printf("Nao existem ficheiros a fazer index!\n");
		return 0;
	}

	char cscCall[PATH_MAX+1];
	sprintf(cscCall,"%s/CSC",realPathArg0);  
	execlp(cscCall,"CSC",realPathArg1,NULL);//calling CSC
	return 0;
}
