#include <pthread.h>
#include "includes.h"

pthread_t threads[4];
int size = 0;

void * handler(void * arg){
	
}

int main(int argc, char** argv){
	char input[256];
	char* args[4];
	int i, j, free = 0;

	for (i = 0; i < 4; i++) threads[i] = 0;

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
				if (threads[i] == 0) {
					free = i;
					break;
				}
			pthread_create(&threads[free], NULL, &handler, NULL);
			size++;
		}
		else if (strcmp(args[0], "kill") == 0){
			pthread_t thread = strtoul(args[1], NULL, 10);
			printf("%lu\n", thread);
			pthread_join(thread, NULL);
			for (i = 0; i < 4; i++) 
				if (threads[i] == thread){
					threads[i] = 0;
					break;
				}
			size--;
		}
		else if (strcmp(args[0], "list") == 0){
			for (i = 0; i < 4; i++) if (threads[i] != 0) printf("Process %lu\n", threads[i]);
		}
		else if (strcmp(args[0], "mem") == 0)
			;
		else if (strcmp(args[0], "read") == 0)
			;
		else if (strcmp(args[0], "write") == 0)
			;
		else if (strcmp(args[0], "exit") == 0)
			exit(0);
	}
	return 0;
}
