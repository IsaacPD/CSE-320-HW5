#include <pthread.h>
#include "includes.h"
#define uint unsigned int
#define MAIN "fifo_main"
#define MEM  "fifo_mem"

typedef struct pt2 {
	uint address[1024];
	int valid[1024];
	int full;
} pt2;

typedef struct pt1 {
	pt2 * table2[1024];
} pt1;

typedef struct process {
	pthread_t tid;
	pt1 table1;
	uint virtuals[1024];
	int size;
} process;

process threads[4];
int size = 0;

uint cse320_malloc(process * p, uint addr){
	uint i1, i2;

	for (i1 = 0; i1 < 1024; i1++){
		if (p->table1.table2[i1] != NULL && p->table1.table2[i1]->full == 0	){
			break;
		}
		else if ((p->table1.table2[i1] == NULL)){
			p->table1.table2[i1] = malloc(sizeof(pt2));
			break;
		}
	}
	for (i2 = 0; i2 < 1024; i2++){
		if (p->table1.table2[i1]->valid[i2] == 0){
			p->table1.table2[i1]->address[i2] = addr;
			p->table1.table2[i1]->valid[i2] = 1;
			break;
		}
	}
	if (i2 == 1024)
		p->table1.table2[i1]->full = 1;

	uint virt = (i1 << 22) | (i2 << 12) | 0;
	return virt;
}

uint cse320_virt_to_phys(uint virt, process p){
	uint t1Index = virt >> 22;
	uint t2Index = (virt >> 12) & 1023;
	pt2 table2 = *(p.table1.table2[t1Index]);
	uint phys = table2.address[t2Index];
	return phys;
}

void * handler(void * arg){
	
}

process * findProcess(pthread_t tid, int * index){
	int i;
	for (i = 0; i < 4; i++){
		if (threads[i].tid == tid){
			if (index != NULL)
				*index = i;
			return &threads[i];
		}
	}
	return NULL;
}

void sendMessage(char * message){
	FILE * mem = fopen(MEM, "w");
	fputs(message, mem);
	fclose(mem);
}

void cleanThreads(){
	int i, j;
	for (i = 0; i < 4; i++){
		if (threads[i].tid == 0) continue;
		for (j = 0; threads[i].table1.table2[j] != NULL; j++)
			free(threads[i].table1.table2[j]);
		pthread_join(threads[i].tid, NULL);
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
	FILE * main;
	
	uint phys;
	char phy[11];
	char input[256], buff[256];
	char* args[4];
	int i, j, result, f = 0;
	process * p;

	for (i = 0; i < 4; i++) {
		threads[i].tid = 0;
		threads[i].size = 0;
	}

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
			if (size >= 4){
				printf("No more processes can be created\n");
				continue;
			}
			for (i = 0; i < 4; i++) 
				if (threads[i].tid == 0) {
					f = i;
					break;
				}
			pthread_create(&(threads[f].tid), NULL, &handler, NULL);
			size++;
		}	
		else if (strcmp(args[0], "kill") == 0){
			pthread_t thread = strtoul(args[1], NULL, 10);
			p = findProcess(thread, &i);
			if (p != NULL){
				pthread_join(thread, NULL);
				sprintf(buff, "kill 0 %d", i);
				sendMessage(buff);
				main = fopen(MAIN, "r");
				fclose(main);
				threads[i].tid = 0;
				for (i = 0; p->table1.table2[i] != NULL; i++){
					free(p->table1.table2[i]);
				}
				size--;
			} else
				printf("That process does not exist\n");
		}
		else if (strcmp(args[0], "list") == 0){
			for (i = 0; i < 4; i++) if (threads[i].tid != 0) printf("Process %lu\n", threads[i].tid);
		}
		else if (strcmp(args[0], "mem") == 0){
			p = findProcess(strtoul(args[1], NULL, 10), NULL);
			if (p == NULL) {
				printf("Not a valid process\n");
				continue;
			}
			int vsize = p->size;
			for (i = 0; i < vsize; i++)
				printf("Virtual Address: 0x%X\n", p->virtuals[i]);
		}
		else if (strcmp(args[0], "allocate") == 0){
			p = findProcess(strtoul(args[1], NULL, 10), &i);
			if (p == NULL) {
				printf("Not a valid process\n");
				continue;
			}
			sprintf(buff, "request 4 %d", i);
			sendMessage(buff);
			main = fopen(MAIN, "r");
			fscanf(main, "%u %s", &phys, buff);
			if (input[0] == 'E') 
				printf("%s for process\n", buff);
			else {
				uint virt = cse320_malloc(p, phys);
				printf("Virtual address 0x%X or %d created for process %s\n", virt, virt, args[1]);
				p->virtuals[p->size] = virt;
				p->size++;
			}
			fclose(main);
		}
		else if (strcmp(args[0], "read") == 0){
			phys = cse320_virt_to_phys(atoi(args[2]), *findProcess(strtoul(args[1], NULL, 10), NULL));
			sprintf(input, "%s %u", args[0], phys);
			sendMessage(input);
			main = fopen(MAIN, "r");
			while(fgets(input, 256, main) > 0);
			printf("%s\n", input);
			fclose(main);
		}
		else if (strcmp(args[0], "write") == 0){
			phys = cse320_virt_to_phys(atoi(args[2]), *findProcess(strtoul(args[1], NULL, 10), NULL));
			sprintf(input, "%s %u %s", args[0], phys, args[3]);
			sendMessage(input);
			main = fopen(MAIN, "r");
			fclose(main);
		}
		else if (strcmp(args[0], "exit") == 0){
			cleanThreads();
			sendMessage("exit");
			main = fopen(MAIN, "r");
			fclose(main);
			exit(0);
		}
	}
	return 0;
}
