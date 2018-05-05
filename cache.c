#include "includes.h"
#define GET_TAG(x) x >> 2
#define GET_SET(x) x & 3

typedef struct block {
	int valid : 1;
	int value : 32;
	int tag : 30;
} block;

block blocks[4];

int main(){
	unsigned int address, old;
	int i, write, result, val, tag;
	FILE * cache, * mem, * main;
	block b;

	char buf[256], command[256];
	while(1){
		cache = fopen(CACHE, "r");
		main = fopen(MAIN, "w");
		sleep(1);
		fscanf(cache, "%s %u %d", command, &address, &write);

		if (strcmp(command, "request") == 0) {
			mem = fopen(MEM, "w");
			sprintf(buf, "%s %u %d %s", command, address, write, CACHE);
			fputs(buf, mem);
			fclose(mem);

			fscanf(cache, "%u", &address);
			b = blocks[GET_SET(address)];
			b.value = 0;
			b.tag = GET_TAG(address);
			b.valid = 1;
			sprintf(buf, "%u", address);
			fputs(buf, main);
			fclose(main);
			fclose(cache);
			continue;
		}
		
		b =  blocks[GET_SET(address)];
		tag = GET_TAG(address);

		if (b.valid == 1){
	   		if (b.tag == tag) result = 0;
			result = 2;
		}
		else result = 1;
		
		if (result == 0){
			printf("cache hit\n");
			val = b.value;
		} else {
			printf("cache miss\n");
			mem = fopen(MEM, "w");
			if (result == 2){
				printf("eviction\n");
				old = GET_SET(address) | (b.tag << 2);
				sprintf(buf, "write %u %d %s", old, b.value, CACHE);
				fputs(buf, mem);
				fclose(mem);
				mem = fopen(MEM, "w");
			}
 			sprintf(buf, "read %u %s", address, CACHE);
			fputs(buf, mem);
			fclose(mem);
			fscanf(cache, "%d", &val);
			b.value = val;
			b.tag = tag;
			b.valid = 1;
		}

		if (strcmp(command, "read") == 0){
			sprintf(buf, "%d", val);
			fputs(buf, main);
		} else if (strcmp(command, "write") == 0){
			b.value = write;
		}

		fclose(main);
		fclose(cache);
	}
}
