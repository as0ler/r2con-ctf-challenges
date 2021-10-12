#include <stdio.h>
#include <stdlib.h>
#include <string.h>

char *read()
{
    int INPUT_LENGTH = 45;
    char *input = malloc(INPUT_LENGTH);

    if (!input)
    {
        fwrite("Could not allocate memory.\n", 1, 27, stderr);
        return 0;
    }
    if (!fgets(input, INPUT_LENGTH, stdin))
    {
        fwrite("Could not read user input.\n", 1, 27, stderr);
        free(input);
        return 0;
    }
    return input;
}

int readAndCheck(char *toCheck)
{
    char *input = read();
    if (!input)
    {
        return 0;
    }
    if (strncmp(input, toCheck, strlen(toCheck)) != 0)
    {
        free(input);
        return 0;
    }
    free(input);
    return 1;
}

int check2(char *input, char *bytes)
{
    char xor [] = {0xde, 0xad, 0xbe, 0xef, 0xde, 0xad, 0xbe, 0xef, 0xca, 0xfe, 0x13, 0x37, 0xca, 0xfe, 0x13, 0x37, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a};
    char add[] = {0x0a, 0x09, 0x08, 0x07, 0x06, 0x05, 0x04, 0x03, 0x02, 0x01, 0x37, 0x13, 0x37, 0x13, 0xfe, 0xca, 0x37, 0x13, 0x37, 0x13, 0xfe, 0xca, 0xef, 0xbe, 0xad, 0xde};
    for (int i = 0; i < strlen(bytes); i++)
    {
        char c = input[i];
        c = c ^ xor[i];
        c = c + add[i];
        if (c != bytes[i])
        {
            return 0;
        }
    }
    return 1;
}

int die()
{
    puts("\nThat was a poor decision and it led to your death. Sorry.");
    return 1;
}

int secretPath(char *input)
{
    char flag[1000];
    strcat(flag, "r2con{");
    strncat(flag, input, 30);
    flag[strlen(flag) - 1] = 0;
    if (!check2(input, "\x97\xcd\xd2\xd6\xc0\xc7\xcd\x84\xec\x91\xad\x62\xf5\xf1\x65\x22\x58\x82\xb1\x37\x61\x3e\x5d\x2b\x14\x4c")) // Sit down next to my friend
    {
        return 2;
    }
    puts("\nYour friend hands you a note. What do you do?");
    input = read();
    strncat(flag, input, 30);
    flag[strlen(flag) - 1] = 0;
    if (!check2(input, "\x9c\xcd\xe1\x8e\xb0\x92\xd7\x91\xc0\x9e\xb2")) // Light match
    {
        return die();
    }
    free(input);
    puts("\nThe note says: \"Don't leave me here\". Do you leave your friend or stay?");
    input = read();
    strncat(flag, input, 30);
    flag[strlen(flag) - 1] = 0;
    if (!check2(input, "\x97\xe2\xe7\x9d")) // Stay
    {
        free(input);
        return die();
    }
    strcat(flag, "}");
    printf("\n%s\n", flag);
    return 0;
}

int main()
{
    puts("--- eXit ---");
    puts("");
    puts("It's time to leave the Land of Ecodelia...");
    puts("Press Enter to continue");
    getchar();

    puts("\nYou're trapped in a dungeon with your friend. You see a barrel. What do you do?");
    char *input = read();
    int ret = secretPath(input);
    if (ret != 2)
    {
        return ret;
    }
    if (strncmp(input, "Move barrel", strlen("Move barrel")) != 0)
    {
        return die();
    }

    puts("\nThe barrel rolls aside and you find a secret tunnel. What do you do?");
    if (!readAndCheck("Enter tunnel"))
    {
        return die();
    }

    puts("\nYou start to escape but your friend is too weak to go with you. They hand you a note. What do you do?");
    if (!readAndCheck("Read note"))
    {
        return die();
    }

    puts("\nIt is to dark to read the note. What do you do?");
    if (!readAndCheck("Leave"))
    {
        return die();
    }

    puts("\nYou crawl through the tunnel and the tunnel leads you to a beach. What do you do?");
    if (!readAndCheck("Look"))
    {
        return die();
    }

    puts("\nIn the water you see a boat. What do you do?");
    if (!readAndCheck("Get on boat"))
    {
        return die();
    }

    puts("\nCongratulations, you're heading to a new world!");
    return 0;
}
