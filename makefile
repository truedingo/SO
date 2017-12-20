all: main pac

main: main.c
	gcc -Wall -pthread -D_REENTRANT main.c -o Main

pac: pac.c
	gcc -Wall pac.c -o Pac
