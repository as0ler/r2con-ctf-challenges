#include <stdio.h>
#include <string.h>

unsigned int keypressed(const char c) {
  return (((7 >> 2) + (c ^ 40) + 20) & 0x5FF5FFF5);
}

int check(const char *str) {
	while (*str) {
		if (*str < 'a' || *str > 'z') return 0;
		str++;
	} 
	return 1;
}
 
const char * secret = "Use ESIL to save the world!"; 

void banner() {

printf("\n");
printf("             . . .                   \n");
printf("              \|/                    \n");                        
printf("            `--+--'                  \n");                       
printf("              /|\                    \n");                       
printf("             ' | '                   \n");                        
printf("               |                     \n");                        
printf("               |                     \n");                        
printf("           ,--'#`--.                 \n");                       
printf("           |#######|                 \n");                       
printf("        _.-'#######`-._              \n");                   
printf("     ,-'###############`-.           \n");                
printf("   ,'#####################`,         \n");              
printf("  /#########################\        \n");            
printf(" |###########################|       \n");           
printf("|#############################|      \n");          
printf("|#############################|      \n");          
printf("|#############################|      \n");        
printf("|#############################|      \n");         
printf(" |###########################|       \n");         
printf("  \#########################/        \n");         
printf("   `.#####################,'         \n");         
printf("     `._###############_,'           \n");         
printf("        `--..#####..--'              \n");
printf("\n");
printf("Insert valid code to defuse the bomb before it explodes: \n");
printf("10, 9, 8, 7, ... \n");

}

void goodboy() {
  printf("[*] You got the flag! You saved the world from the end!\n");
}


void badboy() {
  printf("\n");
  printf("BOOOOM\n");
  printf("     _.-^^---....,,--       \n");
  printf(" _--                  --_   \n");
  printf("<                        >) \n");
  printf("|                         | \n");
  printf(" \._                   _./  \n");
  printf("    ```--. . , ; .--'''     \n");
  printf("          | |   |           \n");
  printf("       .-=||  | |=-.        \n");
  printf("       `-=#$%&%$#=-'        \n");
  printf("          | ;  :|           \n");
  printf(" _____.,-#%&$@%#&#~,._____  \n");
  printf("\n");
}

int main(int argc, char **argv) {
	if (argc != 2) {
		printf ("****** Bomb defusal*******\n");
		banner();
		printf ("Usage: defuse [code]\n");
		return 1;
	}
  
  int flag[20] = {96, 80, 84, 113, 96, 101, 84, 80, 96, 81, 84, 101, 96, 81, 112, 101, 81, 84, 112, 96};
  int res = 1;
  int input_length = strlen(argv[1]);
	if (input_length == 20 && check(argv[1])){ 

    for (int i=input_length; i > 0; i--) {
		  res = res & (keypressed(argv[1][input_length - i]) == flag[i-1]);
    }
		if (res & 0x1) {			
			goodboy();
		} else { 
      badboy();
    }
	}
}
