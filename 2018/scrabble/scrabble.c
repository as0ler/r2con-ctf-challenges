#include <stdio.h>
#include <string.h>

int main(int argc, char** argv) {

    if (argc != 2) {
        printf("Usage: scrabble <flag>\n");
        return -1;
    }

	char* flag = "Wk2p2pm3wg7eo7d";

    int i = 0;
	int x = 0;
	char c;

	while(*flag) {
		c = *flag ^3;
		x = x + strncmp(&c, &argv[1][i],1);
		flag++;
		i++;
	}
    
	if (x) { 
		printf("Nop!\n");	
	} else {
		printf("Yes, r2con{%s} is flag!\n", argv[1]);
    	return 0;
	}
}
