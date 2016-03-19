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

	char wordsPath[200], tempFilePath[200], indexPath[200], indexTempPath[200];
	int pid, status;
	char* realPathArg1 = realpath(argv[1],NULL);
	sprintf(wordsPath, "%s/words.txt", realPathArg1);
	sprintf(indexPath, "%s/index.txt", realPathArg1);
	sprintf(indexTempPath, "%s/indextemp.txt", realPathArg1);

	FILE* indexTemp = fopen(indexTempPath, "w");

	FILE* tempFile;
	FILE* index = fopen(indexPath, "w");
	fwrite("INDEX", 1, 5 * sizeof(char), index);
	int maxTempNumber, tempNumber = 1;
	sprintf(tempFilePath, "%s/%dtemp.txt", realPathArg1, tempNumber);
	//create pipe to use cat
	int fd[2];
	pipe(fd);
	while ((tempFile = fopen(tempFilePath, "r")) != NULL) { // begin tempFiles loop, to concatenate the tempFilex to a single indexTemp
	
		if ((pid = fork()) == 0){ //cat temp files
			dup2(fd[WRITE], STDOUT_FILENO);
			if (execlp("cat", "cat", tempFilePath, NULL) == -1) {
				printf("Erro!");
				return -1;
			}
		}
		else if(pid < 0){
			printf("Ocorreu erro ao executar fork");
			exit(1);
		}
 		else {
			waitpid(pid, &status, 0);
			tempNumber++;
			fclose(tempFile);
			sprintf(tempFilePath, "%s/%dtemp.txt", realPathArg1, tempNumber);
			maxTempNumber = tempNumber;

		}

	} // end of tempFiles opening
	close(fd[WRITE]);
	char nextchar[1];
	while (read(fd[READ], nextchar, sizeof(char)) > 0){
		fwrite(nextchar, 1, sizeof(char), indexTemp);
	}

	fclose(indexTemp); //closing indexTemp writing

	if ((pid = fork()) == 0) //sorting indexTemp
		execlp("sort", "sort", "-fVo",indexTempPath,indexTempPath,NULL);
	else if(pid < 0){
		printf("Ocorreu erro ao executar fork");
		exit(1);
	}
	else
		waitpid(pid, &status, 0);

	char getTempLines[150], wordToCompare[50], currentWord[50];

	wordToCompare[0] = '\0';

	indexTemp = fopen(indexTempPath, "r");

	while (fgets(getTempLines, 150, indexTemp) != NULL) // begin indexTemp searching
	{

		int number, line;
		sscanf(getTempLines, "%s : %d-%d\n", currentWord, &number, &line);

		char toPrint[50];

		if (strcmp(currentWord, wordToCompare) == 0) //compare currentWord with last word
			sprintf(toPrint, ", %d-%d", number, line);
		else {
			sprintf(toPrint, "\n\n%s: %d-%d", currentWord, number, line); // only add word if found at least once
			strcpy(wordToCompare, currentWord);
		}

		fwrite(toPrint, sizeof(char), strlen(toPrint) * sizeof(char), index);

	} // end of indexTemp searching
	fclose(indexTemp); //closing indexTemp reading
	fclose(index); //closing index writing	
	 maxTempNumber--;
	 while(maxTempNumber > 0)//deleting tempFiles cycle
	 {
	 char toDelete[200];
	 sprintf(toDelete, "%s/%dtemp.txt", realPathArg1,maxTempNumber);
	 remove(toDelete);
	 maxTempNumber--;
	 }
	 remove(indexTempPath);//delete indexTemp

	return 0;
}
