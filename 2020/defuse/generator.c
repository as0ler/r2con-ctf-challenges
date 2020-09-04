#include <stdio.h>
#include <string.h>

unsigned int keypressed(const char c) {
  unsigned int hash = 7;
  unsigned int x = 40;
  hash = (hash >> 2) + (c ^ x) + 20;
  return (hash & 0x5FF5FFF5);
}

int check(const char *str) {
	while (*str) {
		if (*str < 'a' || *str > 'z') return 0;
		str++;
	} 
	return 1;
} 
 
int main(int argc, char **argv) {
  //esilrulezonemoretime
  printf("%d, ", keypressed('e'));
  printf("%d, ", keypressed('m'));
  printf("%d, ", keypressed('i'));
  printf("%d, ", keypressed('t'));
  printf("%d, ", keypressed('e'));
  printf("%d, ", keypressed('r'));
  printf("%d, ", keypressed('o'));
  printf("%d, ", keypressed('m'));
  printf("%d, ", keypressed('e'));
  printf("%d, ", keypressed('n'));
  printf("%d, ", keypressed('o'));
  printf("%d, ", keypressed('z'));
  printf("%d, ", keypressed('e'));
  printf("%d, ", keypressed('l'));
  printf("%d, ", keypressed('u'));
  printf("%d, ", keypressed('r'));
  printf("%d, ", keypressed('l'));
  printf("%d, ", keypressed('i'));
  printf("%d, ", keypressed('s'));
  printf("%d, ", keypressed('e'));
}
