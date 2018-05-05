#include "includes.h"
#define GET_TAG(x) x >> 2
#define GET_SET(x) x & 3

typedef struct block {
	int valid;
	int value;
	int tag;
} block;

block blocks[4];

void printBlocks(){
	int i;
	for (i = 0; i < 4; i++){
		printf("%d Value:%d Tag:%d Valid:%d\n", i, blocks[i].value, blocks[i].tag, blocks[i].valid);
	}
	printf("\n");
}

int main(){
	int address, old;
	int i, write, result, val, tag;
	FILE * cache, * mem, * main;
	block* b;

	char buf[256], command[256], source[20];
	while(1){
		printBlocks();
		cache = fopen(CACHE, "r");
		main = fopen(MAIN, "w");
		sleep(1);
		fscanf(cache, "%s %s %u %d", source, command, &address, &write);
		fclose(cache);

		if (strcmp(command, "request") == 0) {
			mem = fopen(MEM, "w");
			sprintf(buf, "%s %s %u %d", CACHE, command, address, write);
			fputs(buf, mem);
			fclose(mem);

			cache = fopen(CACHE, "r");
			fscanf(cache, "%u", &address);
			b = &blocks[GET_SET(address)];
			b->value = 0;
			b->tag = GET_TAG(address);
			b->valid = 1;
			sprintf(buf, "%u", address);
			fputs(buf, main);
			fclose(main);
			fclose(cache);
			continue;
		}
		
		b =  &blocks[GET_SET(address)];
		tag = GET_TAG(address);
		printf("Tag received %d %d", tag, GET_SET(address));

		if (b->valid == 1){
	   		if (b->tag == tag) result = 0;
			result = 2;
		}
		else result = 1;
		
		if (result == 0){
			printf("cache hit\n");
			val = b->value;
		} else {
			printf("cache miss\n");
			mem = fopen(MEM, "w");
			if (result == 2){
				printf("eviction\n");
				old = GET_SET(address) | (b->tag << 2);
				sprintf(buf, "%s write %u %d", CACHE, old, b->value);
				fputs(buf, mem);
				fclose(mem);
				mem = fopen(MEM, "w");
			}
 			sprintf(buf, "%s read %u", CACHE, address);
			fputs(buf, mem);
			fclose(mem);
			cache = fopen(CACHE, "r");
			fscanf(cache, "%d", &val);
			b->value = val;
			b->tag = tag;
			b->valid = 1;
			fclose(cache);
		}

		if (strcmp(command, "read") == 0){
			sprintf(buf, "%d", val);
			fputs(buf, main);
		} else if (strcmp(command, "write") == 0){
			b->value = write;
		}

		fclose(main);
	}
}
