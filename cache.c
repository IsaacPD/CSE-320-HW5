#include "includes.h"
#define GET_TAG(x) x >> 2
#define GET_SET(x) GET_TAG(x) & 3

typedef struct block {
	int valid;
	int value;
	int tag;
	int dirty;
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
	int i, write, result, val, tag, bound;
	int bounds[4] = {256, 512, 768, 1024};
	FILE * cache, * mem, * main;
	block* b;

	char buf[256], command[256], source[20];
	while(1){
		printBlocks();
		cache = fopen(CACHE, "r");
		main = fopen(MAIN, "w");
		fscanf(cache, "%s %s %d %d", source, command, &address, &write);
		fclose(cache);

		if (strcmp(command, "request") == 0) {
			mem = fopen(MEM, "w");
			sprintf(buf, "%s %s %d %d", CACHE, command, address, write);
			fputs(buf, mem);
			fclose(mem);

			cache = fopen(CACHE, "r");
			fscanf(cache, "%d", &address);
			fclose(cache);
			b = &blocks[GET_SET(address)];

			if (b->dirty == 1){
				mem = fopen(MEM, "w");
				printf("eviction\n");
				old = b->tag << 2;
				sprintf(buf, "%s write %d %d", CACHE, old, b->value);
				fputs(buf, mem);
				fclose(mem);
				cache = fopen(CACHE, "r");
				fscanf(cache, "%s", buf);
				fclose(cache);
			}

			b->value = 0;
			b->tag = GET_TAG(address);
			b->valid = 1;
			sprintf(buf, "%d", address);
			fputs(buf, main);
			fclose(main);
			continue;
		}

		if (strcmp(command, "exit") == 0){
			mem = fopen(MEM, "w");
			fputs("fifo_cache exit", mem);
			fclose(mem);
			cache = fopen(CACHE, "r");
			fclose(cache);
			return 0;
		}

		if (strcmp(command, "kill") == 0){
			mem = fopen(MEM, "w");
			sprintf(buf, "%s %s %d %d", CACHE, command, address, write);
			fputs(buf, mem);
			fclose(mem);
			bound = bounds[write];
			for (i = 0; i < 4; i++){
				if ((blocks[i].tag << 2) < bound)
					blocks[i].valid = 0;
			}
			fclose(main);
			continue;
		}
		
		b =  &blocks[GET_SET(address)];
		tag = GET_TAG(address);
		printf("Tag received %d %d\n", tag, GET_SET(address));

		if (b->valid == 1){
	   		if (b->tag == tag) result = 0;
			else result = 2;
		}
		else result = 1;
		
		if (result == 0){
			printf("cache hit\n");
			val = b->value;
		} else {
			printf("cache miss\n");
			mem = fopen(MEM, "w");
			if (b->dirty == 1){
				printf("eviction\n");
				old = b->tag << 2;
				sprintf(buf, "%s write %d %d", CACHE, old, b->value);
				fputs(buf, mem);
				fclose(mem);
				cache = fopen(CACHE, "r");
				fscanf(cache, "%s", buf);
				fclose(cache);
				mem = fopen(MEM, "w");
			}
 			sprintf(buf, "%s read %d", CACHE, address);
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
			b->dirty = 1;
			fputs("write", main);
		}

		fclose(main);
	}
}
