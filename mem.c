#include "includes.h"
#define MAIN "fifo_main"
#define MEM  "fifo_mem"

int main(){
	void * memory = calloc(1024, 1);
	FILE * main, * mem;
	unsigned int address;
	int write, i, bound;
	
	int points[4] = {0, 256, 512, 768};

	char buf[250];
	while(1){
		mem = fopen(MEM, "r");
		main = fopen(MAIN, "w");
		
		fscanf(mem, "%s %u %d", buf, &address, &write);
		if (strcmp(buf, "exit") == 0) {
			free(memory);
			fclose(mem);
			fclose(main);
			return 0;
		}
		sleep(5);
		printf("%s %u %d\n", buf, address, write);
		if (address % 4 != 0){
			printf("error,address is not aligned\n");
		} else if (address >= 1024){
			printf("error,address out of range\n");
		}
		else if (strcmp(buf, "read") == 0) {
			sprintf(buf, "%d", (*(int *)(memory + address)));
			fputs(buf, main);
		} else if (strcmp(buf, "write") == 0){
			(*(int*)(memory + address)) = write;
		} else if (strcmp(buf, "request") == 0){
			i = points[write];
			if (i < 256 * (write + 1)){
				sprintf(buf, "%d", i);
				fputs(buf, main);
				points[write] = i + 4;
			}
			else{
				fputs("Error No Memory", main);
			}
		} else if (strcmp(buf, "kill") == 0){
			points[write] = write * 256;
		}
		printf("received %s\n", buf);
		fclose(mem);
		fclose(main);
	}
	return 0;
}
