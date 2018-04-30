all:
	gcc -g main.c -o main -pthread
	gcc -g mem.c -o mem -pthread
