all : SW.c index.c CSC.c
	gcc -Wall index.c -o bin/index
	gcc -Wall SW.c -o bin/SW
	gcc -Wall CSC.c -o bin/CSC
