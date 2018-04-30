#include <pthread.h>
#include "includes.h"
#define uint unsigned int
#define MAIN "fifo_main"
#define MEM  "fifo_mem"

typedef struct pt2 {
	uint * address;
	int valid;
} pt2;

typedef struct pt1 {
	pt2 * table2;
	int valid;
} pt1;

typedef struct process {
	pthread_t tid;
	pt1 table1;
} process;

process threads[4];
int size = 0;

uint cse320_malloc(){
	return 0;
}

uint cse320_virt_to_phys(uint virt, process p){

	return 0;
}

void * handler(void * arg){
	
}

process findProcess(pthread_t tid){
	int i;
	for (i = 0; i < 4; i++){
		if (threads[i].tid == tid)
			return threads[i];
	}
}

int main(int argc, char** argv){
	//Make FIFO
	umask(0);
	if (mkfifo(MAIN, 0666) < 0 || mkfifo(MEM, 0666) < 0) {
		if (errno != EEXIST){
			perror("FIFO");
			exit(0);
		}
	}
	FILE * main = fopen(MAIN, "r");
	FILE * mem = fopen(MEM, "w");
	char buf[250];
	while(1){
		printf("prompt> ");
		scanf(" %s", buf);
		fputs(buf, mem);
	}
	
	uint phys;
	char phy[11];
	char input[256];
	char* args[4];
	int i, j, result, free = 0;

	for (i = 0; i < 4; i++) threads[i].tid = 0;

	while(1){
		printf("prompt> ");
		fgets(input, 256, stdin);

		j = 1;
		for (i = 0; input[i] != '\0'; i++){
			if (input[i] == ' ' && j < 4){
				input[i] = '\0';
				args[j] = &input[i+1];
				j++;
			}
		}
		input[i-1] = '\0';
		
		args[0] = &input[0];

		if (strcmp(args[0], "create") == 0){
			if (size >= 4) {
				printf("No more processes can be created\n");
				break;
			}
			for (i = 0; i < 4; i++) 
				if (threads[i].tid == 0) {
					free = i;
					break;
				}
			pthread_create(&threads[free].tid, NULL, &handler, NULL);
			size++;
		}
		else if (strcmp(args[0], "kill") == 0){
			pthread_t thread = strtoul(args[1], NULL, 10);
			printf("%lu\n", thread);
			pthread_join(thread, NULL);
			for (i = 0; i < 4; i++) 
				if (threads[i].tid == thread){
					threads[i].tid = 0;
					break;
				}
			size--;
		}
		else if (strcmp(args[0], "list") == 0){
			for (i = 0; i < 4; i++) if (threads[i].tid != 0) printf("Process %lu\n", threads[i].tid);
		}
		else if (strcmp(args[0], "mem") == 0){
			;
		}
		else if (strcmp(args[0], "allocate") == 0){
			cse320_malloc();
		}
		else if (strcmp(args[0], "read") == 0){
			phys = cse320_virt_to_phys(atoi(args[1]), args[2]);
			sprintf(phy, "%d", phys);
			fputs(phy, mem);
			fscanf(main, "%d", &result);
			printf("%d", result);
		}
		else if (strcmp(args[0], "write") == 0)
			;
		else if (strcmp(args[0], "exit") == 0){
			fclose(main);
			fclose(mem);
			exit(0);
		}
	}
	return 0;
}
