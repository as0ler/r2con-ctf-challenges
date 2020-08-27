#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct _globals {
	char userEchoRequest[512];
	char flagBuffer[256];
} globals;

int main(int argc, char ** argv)
{
	FILE *flagFile;
	char *tmp;
	int bytesnum;
	
	char *line_buf = NULL;
	size_t line_buf_size = 0;
	ssize_t bytes_read;

	char flag_path[9];
       	flag_path[0] = 'f';
	flag_path[1] = 'l';
	flag_path[2] = 'a';
	flag_path[3] = 'g';
	flag_path[4] = '.';
	flag_path[5] = 't';	
	flag_path[6] = 'x';	
	flag_path[7] = 't';	
	flag_path[8] = 0;	

	setvbuf(stdout, NULL, _IONBF, 0);
	setvbuf(stdin, NULL, _IONBF, 0);

	bzero(globals.flagBuffer, 256);
	flagFile = fopen(flag_path, "r");
	
	if(flagFile == NULL) {
		puts("Could not open the flag.");
		return -1;
	}

	fread(globals.flagBuffer, 1, 256, flagFile);
	while(1) {
	        bytes_read = getline(&line_buf, &line_buf_size, stdin);
		
		tmp = strtok(line_buf, "!");
		if(strcmp(tmp, "CLIENT HELLO") != 0) {
			puts("500 - Server Error");
			exit(-1);
		}

		puts("hello \\o/");

	        bytes_read = getline(&line_buf, &line_buf_size, stdin);
		tmp = strtok(line_buf, "?");
		if(strcmp(tmp, "ECHO REQUEST") != 0) {
			puts("500 - Server Error");
			exit(-1);
		}

		puts("Yes I am here");

		bytes_read = getline(&line_buf, &line_buf_size, stdin);
		tmp = strtok(line_buf, "<");
		if(strcmp(tmp, "PLEASE REPLY ") != 0) {
			puts("500 - Server Error");
			exit(-1);
		}

		tmp = strtok(NULL, ">");
		memcpy(globals.userEchoRequest, tmp, strlen(tmp));
		
		tmp = strtok(NULL, "[");
		tmp = strtok(NULL, "]");
		sscanf(tmp, "%d BYTES", &bytesnum);

		globals.userEchoRequest[bytesnum] = 0;
		if(bytesnum > strlen(globals.userEchoRequest)) {
			puts("Segmentation Fault (core dumped)");
			exit(-1);
		}

		printf("%s\n", globals.userEchoRequest);
	}
}
