#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "rc4.c"

void main(int argc, char **argv)
{
    unsigned char input[4096];
    unsigned char output[4096];
    unsigned char control[4096];
    char c = 0;
    int i = 0;
    int r = 0;

    freopen(NULL, "rb", stdin);
    freopen(NULL, "wb", stdout);

    if (argc != 2) exit(1);
    if (strlen(argv[1]) != 1) exit(1);

    memset(input, 0, sizeof(input));
    while ((c = getchar()) != EOF) {
        input[i] = (unsigned char) c;
        i++;
        if (i > sizeof(input)) exit(1);
    }

    memset(output, 0, sizeof(output));
    RC4((unsigned char *) argv[1], (unsigned char *) &input, i, (unsigned char *) &output);

    memset(control, 0, sizeof(control));
    RC4((unsigned char *) argv[1], (unsigned char *) &output, i, (unsigned char *) &control);
    r = memcmp(input, control, i);
    if (r != 0) {
        fprintf(stderr, "ERROR 1");
        exit(1);
    }

    memset(control, 0, sizeof(control));
    RC4((unsigned char *) argv[1], (unsigned char *) &input, i, (unsigned char *) &control);
    r = memcmp(output, control, i);
    if (r != 0) {
        fprintf(stderr, "ERROR 2");
        exit(1);
    }

    fwrite(output, 1, i, stdout);
    exit(0);
}
