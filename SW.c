#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <errno.h>
#include <string.h>
#include <fcntl.h>
#include <sys/wait.h>

#define READ 0
#define WRITE 1

int main(int argc, char *argv[]) {
	//create paths to open several files
	char temp1[200], temp2[200], temp3[200];
	int i = *argv[1];
	char* realPathArg2 = realpath(argv[2],NULL);
	sprintf(temp1, "%s/%d.txt", realPathArg2, i);
	sprintf(temp2, "%s/%dtemp.txt", realPathArg2, i);
	sprintf(temp3, "%s/words.txt", realPathArg2);
	FILE *temp;
	//create tempFile number i
	if ((temp = fopen(temp2, "w")) == NULL){
		return -1;
	}
	else {
		//open words.txt
		FILE *words = fopen(temp3, "r");
		char word[50];
		while (fgets(word, 50, words) != NULL) {
			int size = strlen(word);
			word[size - 1] = '\0';
			char toprint[50];
			int pid;
			//create pipe to use grep
			int fd[2];
			pipe(fd);
			
			if ((pid = fork()) == 0) {
				close(fd[READ]);
				dup2(fd[WRITE], STDOUT_FILENO);
				if (execlp("grep", "grep", "-w", "-n", "-o", word, temp1, NULL)// use of grep
						== -1) {
					printf("Erro!");
					return -1;
				}
			}
			else if(pid < 0){
				printf("Ocorreu erro ao executar fork!");
				exit(1);
			}
			close(fd[WRITE]);
			int status;
			waitpid(pid,&status,0);
			char nextchar[1];
			int counter = 0;
			char grepstr[500];
			while (read(fd[READ], nextchar, sizeof(char)) > 0) {// reading from pipe what grep has written
				if (nextchar[0] == '\n') {// writing which and where a word was found
		 			grepstr[counter] = '\0';
		 			char towrite[100], str_garbage[250];
					sprintf(toprint, "%s : ", word);
					fwrite(toprint, 1, strlen(toprint), temp);
					int line = 0;
					sscanf(grepstr, "%d:%s", &line, str_garbage);
					sprintf(towrite, "%d-%d", i, line);
					fwrite(towrite, 1, strlen(towrite), temp);
					fwrite("\n", sizeof(char), 1, temp);
					counter = 0;
				 }
			else{
				grepstr[counter] = nextchar[0];
				counter++;
			}
		}
		close(fd[READ]);
		}
	}
	exit(0);
}
