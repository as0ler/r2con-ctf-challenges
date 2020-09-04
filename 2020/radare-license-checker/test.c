#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "rc4.c"

int main(int argc, unsigned char *argv[]) {
    unsigned char key[1024];
    unsigned char plaintext[1024];
    unsigned char ciphertext[1024];
    unsigned char control[1024];
    int result;

    memset(key, 0, sizeof(key));
    memset(plaintext, 0, sizeof(plaintext));
    memset(ciphertext, 0, sizeof(ciphertext));
    memset(control, 0, sizeof(control));

    strcpy(key, "this is a test key");
    strcpy(plaintext, "this is a test string");

    RC4(key, plaintext, strlen(plaintext), ciphertext);
    RC4(key, ciphertext, strlen(plaintext), control);
    result = memcmp(plaintext, control, sizeof(plaintext));
    if (result != 0) {
        printf("FAILED 1\n");
        return 1;
    }
    RC4(key, plaintext, strlen(plaintext), control);
    result = memcmp(ciphertext, control, sizeof(plaintext));
    if (result != 0) {
        printf("FAILED 2\n");
        return 1;
    }
    printf("PASSED\n");
    return 0;
}
