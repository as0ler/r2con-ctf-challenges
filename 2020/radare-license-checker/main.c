#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>

#include "main.h"
#include "rc4.h"
#include "flag.h"

void main(int argc, char **argv)
{
    size_t license_len;
    unsigned char *license;

    printf("Radare License Checker\n");

    if (argc < 2) {
        printf("ERROR: no license provided!\n\nUsage: radarelicensechecker.exe <license>\n\n");
        exit(1);
    }

    if (argc != 2) {
        printf("ERROR: too many arguments!\n\nUsage: radarelicensechecker.exe <license>\n\n");
        exit(1);
    }

    license_len = sizeof(chunks) / sizeof(chunks[0]);
    license = (unsigned char *) argv[1];
    if (strlen(license) != license_len) {
        printf("ERROR: wrong size of license string!\n\nUsage: radarelicensechecker.exe <license>\n\n");
        exit(1);
    }

    printf("Decoding license data...\n\n");
    for (int i = 0; i < license_len; i++) {
#ifdef DO_SLEEP
        sleep(1);
#endif
        unsigned char out[sizeof(chunk_0)];
        unsigned char keystr[2];
        keystr[0] = license[i];
        keystr[1] = 0;
        memset(out, 0, sizeof(out));
        RC4((unsigned char *) &keystr, chunks[i], sizeof(out), (unsigned char *) &out);
        if (out[0] == 0 || out[sizeof(out) - 1] != 0) {
            printf("\n\nERROR: decryption error in block %i!\n\n", i);
            exit(1 + i);
        }
        for (int j = 0; j < sizeof(out); j++) {
            if (out[j] == 0) break;
            if ( !isprint(out[j]) && out[j] != ' ' && out[j] != '\t' && out[j] != '\r' && out[j] != '\n' ) {
                printf("\n\nERROR: decryption error in block %i!\n\n", i);
                exit(1 + i);
            }
        }
        printf("%s", out);
    }
}
