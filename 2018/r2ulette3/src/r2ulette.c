#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define LEN 256
#define ADDR "0.0.0.0"
#define PORT 7878
#define CONNQUEUE 128

void roulette();

int conn;
FILE *fd;

void banner() {
fprintf(fd, "                   .------.\n");
fprintf(fd, ".------.           |A .   |\n");
fprintf(fd, "|A_  _ |    .------; / \\  |\n");
fprintf(fd, "|( \\/ )|-----. _   |(_,_) |\n");
fprintf(fd, "| \\  / | /\\  |( )  |  I  A|\n");
fprintf(fd, "|  \\/ A|/  \\ |_x_) |------'\n");
fprintf(fd, "`-----+'\\  / | Y  A|       \n");
fprintf(fd,"      |  \\/ A|-----'       \n");
fprintf(fd,"      `------'             \n"); 	

}

void roulette() {
	char input[LEN], *ptr;
	int len, n;

                                                                        
	banner();
	fprintf(fd, "Interested in playing R2ulette? Win playing our Free Roulette game today and go build your own theme park, with blackjack and hookers!\n\n");

	srand(time(NULL));
	n = rand() % 100;
	for (;;) {
		fprintf(fd, "Pick a number (0 - 100), and good luck!: ");
		fflush(fd);
		if (fgets(input, LEN*4, fd) == NULL)
			break;

		if ((ptr = strchr(input, '\n')) != NULL)
			*ptr = '\x00';

		if (input[0] == 'q')
			break;
		
		fprintf(fd, "Your got:  ");
		fprintf(fd, input);

		if (atoi(input) != n) {
			fprintf(fd, "\n Bad Luck, you are not the winner\n");
			fflush(fd);
		} else {
			fprintf(fd, "\n Yay \\o/! Congratulations!\n\n");
			fflush(fd);
			break;
		}
	}
	fprintf(fd, "\n");
	fflush(fd);
}

int main(int argc, char **argv) {
	struct sockaddr_in srvaddr, cliaddr;
	int sock, pid;
	unsigned int cliaddrlen = sizeof(cliaddr);

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
		conn = accept(sock, (struct sockaddr *)&cliaddr, &cliaddrlen);
		pid = fork();

		if(pid == 0) {
			alarm(60);
			if ((fd = fdopen(conn, "r+")) == NULL) {
				perror("fdopen");
				close(conn);
				break;
			}
			roulette();
			fclose(fd);
			close(conn);
			break;
		} else {
			close(conn);
			continue;
		}
	}
	close(sock);
	return 0;
}
