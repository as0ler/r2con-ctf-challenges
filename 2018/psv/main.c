#include <stdio.h>
#include <stdlib.h>

void printHeader() {
	puts("Welcome to PSV (Perfectly Secure Vault)!");
	puts("Enter your secret key to unlock:");
}

int checkInput(char* input, int inputLength) {
	char* THIS_IS_NOT_THE_FLAG = "sorry_this_is_not_the_flag_u_are_looking_for";
	char SECRET_KEY[] = { 0x1, 0x5d, 0x11, 0x1d, 0x17, 0x24, 0xc, 0x58, 0x1b, 0x2c, 0x6e, 0x1a, 0x2c, 0xc, 0x1b, 0x1f, 0x47, 0x2d, 0x2b, 0x23, 0x55, 0x6f, 0xa, 0x33, 0x47, 0x38, 0x2c, 0x46, 0x3c, 0x14, 0x0, 0x56, 0x0, 0x58, 0x3, 0x8, 0x5b, 0x1b, 0x5f, 0x13, 0x37, 0xb, 0x4e, 0xf };
	
	int i;
	for (i=0; i<inputLength; i++) {
		if (THIS_IS_NOT_THE_FLAG[i] != (input[i] ^ SECRET_KEY[i]))
			return 0;
	}
	return 1;
}

int main() {
	
	printHeader();
	
	int INPUT_LENGTH = 45;
	char* input = malloc(INPUT_LENGTH);
	
	if (!input) {
		fwrite("Could not allocate memory.\n", 1, 27, stderr);
		return 1;
	}
	if (!fgets(input, INPUT_LENGTH, stdin)) {
		fwrite("Could not read user input.\n", 1, 27, stderr);
		free(input);
		return 1;	
	}
	if (checkInput(input, INPUT_LENGTH - 1)) {
		puts("Correct! You got the flag!");
	} else {
		fwrite("Sorry, try again.\n", 1, 18, stderr);
	}
	free(input);
	return 0;
}

