all: main mem cache

main: main.c
	gcc -g main.c -o main -pthread
mem: mem.c
	gcc -g mem.c -o mem
cache: cache.c
	gcc -g cache.c -o cache
clean:
	-rm main mem cache
