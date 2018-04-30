#include "includes.h"
#define MAIN "fifo_main"
#define MEM  "fifo_mem"

int main(){
	sleep(5);
	void * memory = calloc(1024, 1);

	FILE * main = fopen(MAIN, "w");
	FILE * mem;
	char buf[250];
	while(1){
		mem = fopen(MEM, "r");
		fgets(buf, 80, mem);
		printf("received %s\n", buf);
		fclose(mem);
	}
	return 0;
}
