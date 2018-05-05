all: main mem cache part2

main: main.c
	gcc -g main.c -o main -pthread
mem: mem.c
	gcc -g mem.c -o mem
cache: cache.c
	gcc -g cache.c -o cache
part2: part2.c
	gcc -g part2.c -o part2 -pthread
clean:
	-rm main mem cache part2
