#include <string.h>

#define N 256   // 2^8

void swap(unsigned char *a, unsigned char *b) {
    int tmp = *a;
    *a = *b;
    *b = tmp;
}

int KSA(unsigned char *key, unsigned char *S) {

    int len = strlen(key);
    int j = 0;

    for(int i = 0; i < N; i++)
        S[i] = i;

    for(int i = 0; i < N; i++) {
        j = (j + S[i] + key[i % len]) % N;

        swap(&S[i], &S[j]);
    }

    return 0;
}

int PRGA(unsigned char *S, unsigned char *plaintext, size_t len, unsigned char *ciphertext) {

    int i = 0;
    int j = 0;

    for(size_t n = 0; n < len; n++) {
        i = (i + 1) % N;
        j = (j + S[i]) % N;

        swap(&S[i], &S[j]);
        int rnd = S[(S[i] + S[j]) % N];

        ciphertext[n] = rnd ^ plaintext[n];

    }

    return 0;
}

int RC4(unsigned char *key, unsigned char *plaintext, size_t len, unsigned char *ciphertext) {

    unsigned char S[N];
    memset(S, 0, sizeof(S));
    KSA(key, S);
    PRGA(S, plaintext, len, ciphertext);
    return 0;
}
