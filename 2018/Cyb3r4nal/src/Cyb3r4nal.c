#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>
#include <time.h>
#include <stdlib.h>


#define NCMD 13
#define NVAR 1024
#define STRSZ 256
#define ADDR "0.0.0.0"
#define PORT 1337
#define CONNQUEUE 128

enum ret_code {
	CONT,
	EXIT,
	UNK
};

struct command {
	const char *cmd;
	int (*fcn)(char *args);
};

struct var {
	char name[STRSZ];
	char value[STRSZ];
};

int cmd_export(char *args);
int cmd_man(char *args);
int cmd_anal(char *args);
int cmd_cat(char *args);
int cmd_echo(char *args);
int cmd_env(char *args);
int cmd_printenv(char *args);
int cmd_exit(char *args);
int cmd_help(char *args);
int cmd_ls(char *args);
int cmd_set(char *args);
int cmd_unset(char *args);
int cmd_g0d(char *args);
void print_banner();
char* get_cmd(char *cmd, int len);
int parse_cmd();

FILE *fdclient;
int admin;
int curvar;

struct command commands[NCMD] = {
	{ "export", &cmd_export },
	{ "anal", &cmd_anal },
	{ "cat", &cmd_cat },
	{ "echo", &cmd_echo },
	{ "printenv", &cmd_printenv },
	{ "env", &cmd_env },
	{ "exit", &cmd_exit },
	{ "help", &cmd_help },
	{ "man", &cmd_man },
	{ "ls", &cmd_ls },
	{ "set", &cmd_set },
	{ "unset", &cmd_unset },
	{ "su", &cmd_g0d }
};

struct var vars[NVAR];

int cmd_anal(char *args) {

	if(!args)
		return CONT;

	fprintf(fdclient, "[*] You Got (__X__)\n"); 

	return CONT;
}

int cmd_export(char *args) {
	char *ptr, *orig = NULL, *dest = NULL;
	int   i, o = -1, d = -1;

	if (!args) {
		fprintf(fdclient, "usage: export <orig> <dest>\n");
		return CONT;
	}

	if ((ptr = strchr(args, ' ')) == NULL) {
		fprintf(fdclient, "usage: export <orig> <dest>\n");
		return CONT;
	} else {
		*ptr = '\0';
	}
	
	orig = args;
	dest = ptr+1;

	for (i = 0; i < curvar; i++) {
		if (strcmp(vars[i].name, orig) == 0) {
			o = i;		
			break;
		}
	}

	for (i = 0; i < curvar; i++) {
		if (strcmp(vars[i].name, dest) == 0) {
			d = i;
			break;
		}
	}
	strncpy(vars[d].value, vars[o].value, 1);

	if (o < 0 || d < 0) {
			fprintf(fdclient, "error: cannot add more env vars\n");
	}

	return CONT;

}


int cmd_cat(char *args) {
	FILE *fd;
	char buf[STRSZ];

	if (!args)
		return CONT;

	if (!admin) {
		fprintf(fdclient, "error: permission denied\n");
		return CONT;
	}

	if (strcmp(args, "flag.txt") != 0) {
		fprintf(fdclient, "error: permission denied\n");
		return CONT;
	}

	fd = fopen("flag.txt", "r");
	if (fd == NULL) {
		fprintf(fdclient, "error: cannot open flag.txt\n");
		return CONT;
	}
	fgets(buf, STRSZ, fd);
	fprintf(fdclient, "%s", buf);
	fclose(fd);

	return CONT;
}

int cmd_man(char *args) {

	srand(time(NULL));	
	int r = rand() % 5;
	switch(r) {
		case 0:
			fprintf(fdclient, "[+] Stop using anal as a variable!\n");
			break;
		case 1:
			fprintf(fdclient, "[+] Anal jokes never get old\n");
			break;
		case 2:
			fprintf(fdclient, "[+] Wherever I look I see anal this and anal that!\n");
			break;
		case 3:
			fprintf(fdclient, "[+] Wherever Undefined symbol 'r_anal_fisting'.\n");
			break;
		default:
			fprintf(fdclient, "[+] ¯_(ツ)_/¯ There's some hidden var'.\n");
			break;
	}
	return CONT;
}

int cmd_echo(char *args) {
	if (args) fprintf(fdclient, "%s\n", args);
	return CONT;
}

int cmd_printenv(char *args) {
	int i;
	char *name;

	if (!args) {
		fprintf(fdclient, "usage: set <var> <value>\n");
		return CONT;
	}

	name = args;
	for (i = 0; i < curvar; i++) {
		if (strcmp(vars[i].name, name) == 0) {
			fprintf(fdclient, "%s: %s\n", vars[i].name, vars[i].value);
			break;
		}
	}

	return CONT;
}

int cmd_env(char *args) {
	int i;

	for (i = 1; i < curvar; i++) {
		if (vars[i].name[0] == '\0') continue;
		fprintf(fdclient, "%s: %s\n", vars[i].name, vars[i].value);
	}
	return CONT;
}

int cmd_exit(char *args) {
	return EXIT;
}

int cmd_help(char *args) {
	fprintf(fdclient, "available commands:\n");
	fprintf(fdclient, "* cat echo printenv env exit help ls set unset man anal ...\n");
	return CONT;
}

int cmd_ls(char *args) {
	DIR *dir;
	struct dirent *d;
	
	dir = opendir(".");
	if (dir == NULL) {
		fprintf(fdclient, "error: cannot open dir");
		return CONT;
	}
	while ((d = readdir(dir)) != NULL) {
		fprintf(fdclient, "%s\n", d->d_name);
	}
	closedir(dir);
	return CONT;
}

int cmd_set(char *args) {
	char *ptr, *name = NULL, *value = NULL;
	int dup = 0, i;

	if (!args) {
		fprintf(fdclient, "usage: set <var> <value>\n");
		return CONT;
	}

	if ((ptr = strchr(args, ' ')) == NULL) {
		fprintf(fdclient, "usage: set <var> <value>\n");
		return CONT;
	} else {
		*ptr = '\0';
	}
	
	name = args;
	value = ptr+1;

	for (i = 0; i < curvar; i++) {

		if (strcmp(name, "Admin") == 0) {
			fprintf(fdclient, "error: permission denied\n");
			return CONT;
		}

		if (strcmp(vars[i].name, name) == 0) {
			strncpy(vars[i].value, value, STRSZ);
			dup = 1;
			break;
		}
	}

	if (!dup) {
		if (curvar > NVAR-1) {
			fprintf(fdclient, "error: cannot add more env vars\n");
			return CONT;
		}
		strncpy(vars[curvar].name, name, STRSZ);
		strncpy(vars[curvar].value, value, STRSZ);
		curvar++;
	}

	return CONT;
}

int cmd_unset(char *args) {
	int i;

	if (!args) {
		fprintf(fdclient, "usage: unset <var>\n");
		return CONT;
	}

	for (i = 0; i < curvar; i++) {
		if (strcmp(args, "Admin") == 0) {
			fprintf(fdclient, "error: permission denied\n");
			break;
		}
		if (strcmp(vars[i].name, args) == 0) {
			vars[i].name[0] = '\0';
			break;
		}
	}
	return CONT;
}

int cmd_g0d(char *args) {
	int i;

	for (i = 0; i < curvar; i++) {
		if (strcmp(vars[i].name, "Admin") == 0 &&
			strcmp(vars[i].value, "1") == 0)  {
			admin = 1;
			break;
		}
	}
	return CONT;
}

void print_banner() {
	fprintf(fdclient,
			"#############################################################\n"
			"#                 Welcome to Cyb3r4nal.gov                  #\n"
			"#        All connections are monitored and recorded         #\n"
			"#               Load information is forbidden               #\n"
			"# Disconnect IMMEDIATELY if you are not an authorized user, #\n"
			"#               specially for R2Con Attendees               #\n"
			"#############################################################\n\n"
		  );
}

char* get_cmd(char *cmd, int len) {
	fprintf(fdclient, "%c ", admin ? '#' : '$');
	return fgets(cmd, len, fdclient);
}

int parse_cmd(char *cmd) {
	int i;
	char *ptr, *args = NULL;

	if ((ptr = strchr(cmd, '\n')) != NULL)
		*ptr = '\0';

	if ((ptr = strchr(cmd, ' ')) != NULL) {
		*ptr = '\0';
		args = ptr + 1;
	}

	for (i = 0; i < NCMD; i++) {
		if (strcmp(cmd, commands[i].cmd) != 0)
			continue;
		return commands[i].fcn(args); 
	}
	return UNK;
}

void init_globals() {
	int i;
	
	admin = 0;

	curvar = 0;
	for (i = 0; i < NVAR; i++) {
		vars[i].name[0] = 0;
		vars[i].value[0] = 0;
	}
		
	strncpy(vars[curvar].name, "Admin", STRSZ); 
	strncpy(vars[curvar].value, "0", STRSZ);
	curvar++;
}

int loop() {
	char cmd[STRSZ];
	int ret;

	init_globals();

	print_banner();
	for (;;) {
		if (get_cmd(cmd, STRSZ) == NULL)
			break;
		ret = parse_cmd(cmd);
		switch (ret) {
		case CONT:
			continue;
		case EXIT:
			goto _exit;
		default:
			fprintf(fdclient, "Unknown cmd\n");
		}
	}
_exit:
	fprintf(fdclient, "Bye\n");
	return CONT;
}

int main(int argc, char **argv) {
	struct sockaddr_in srvaddr, cliaddr;
	int client, sock, pid;
	socklen_t cliaddrlen = sizeof(cliaddr);

    memset(&srvaddr, '0', sizeof(srvaddr));
    srvaddr.sin_family = AF_INET;
    srvaddr.sin_port = htons(PORT);
    srvaddr.sin_addr.s_addr = inet_addr(ADDR);

    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
        perror("socket");
		return 1;
    }

    if (bind(sock, (const struct sockaddr*)&srvaddr, sizeof(srvaddr)) == -1) {
        perror("bind");
		return 1;
    }

    if (listen(sock, CONNQUEUE) == -1) {
        perror("listen");
		return 1;
    }

	for(;;) {
		client = accept(sock, (struct sockaddr *)&cliaddr, &cliaddrlen);
		pid = fork();

		if (pid < 0) {
			close(client);
			continue;
		} else if (pid > 0) {
			close(client);
			continue;
		} else if(pid == 0) {
			alarm(60);
			if ((fdclient = fdopen(client, "r+")) == NULL) {
				perror("fdopen");
				close(client);
				break;
			}
			close(sock);
			loop();
			fclose(fdclient);
			close(client);
			break;
		}
	}
	if (pid != 0) close(sock);
	return 0;
}
