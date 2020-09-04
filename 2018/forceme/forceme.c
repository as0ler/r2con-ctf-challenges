#include <stdio.h>
#include <string.h>

unsigned int code(const char c) {
  unsigned int hash = 7;
  unsigned int x = 69;
  hash = (hash << 4) + (c ^ x);
  return (hash & 0x7FFFFFFF);
}

int check(const char *str) {
	while (*str) {
		if (*str < 'a' || *str > 'z') return 0;
		str++;
	} 
	return 1;
}
 
const char * secret = "Force Me, and use ESIL on me, dirty boy!"; 

void banner() {

printf("\n");
printf("    .----.\n");
printf("   / / .--.\\ \n");
printf("  / /      \\ \n");
printf(" | |       | |\n");
printf(" | |.----. | |\n");
printf("/// .::::. \\ \\\n");
printf("||| ::/  ::  |\n");
printf("||; ::\\__/:: | \n");
printf("\\ '::::' /  /\n");
printf("  `=':-..-'` \n");
}

void goodboy() {

printf("\n");
printf("    .-----\n");
printf("   / / .-/ \n");
printf("  / /      \n");
printf(" | |        -\n");
printf(" | |.----. | |\n");
printf("/// .::::. \\ \\\n");
printf("||| ::/  ::  |\n");
printf("||; ::\\__/:: | \n");
printf("\\ '::::' /  /\n");
printf("  `=':-..-'` \n");
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf ("****** As a Lockpicking master, Open the Lock! *******\n");
		banner();
		printf ("Usage: forceme [code]\n");
		return 1;
	}

 	int res = 1;
	if (strlen(argv[1]) == 9 && check(argv[1])){ 
		res = res & (code(argv[1][0]) == 144);
		res = res & (code(argv[1][1]) == 166);
		res = res & (code(argv[1][2]) == 156);
		res = res & (code(argv[1][3]) == 153);
		res = res & (code(argv[1][4]) == 167);
		res = res & (code(argv[1][5]) == 160);
		res = res & (code(argv[1][6]) == 153);
		res = res & (code(argv[1][7]) == 144);
		res = res & (code(argv[1][8]) == 175);
		
		if (res) { 			
			printf("%s\n", "Success! You Got it!");
			goodboy();
		} else { printf("%s\n", "Try harder");}
	}
}
