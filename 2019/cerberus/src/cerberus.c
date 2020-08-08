#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <signal.h>

#define NCMD 10
#define NVAR 1024
#define STRSZ 256
#define ADDR "0.0.0.0"
#define PORT 1337
#define CONNQUEUE 128
#define BUF_SIZE 40

enum ret_code {
  OK,
  EXIT,
  DENIED,
  UNAUTH,
  INVALID
};

struct cmd_handler {
  const char *cmd;
  int (*fcn)(char *args);
};

struct var {
  char name[STRSZ];
  char value[STRSZ];
};

int cmd_exit(char *args);
int cmd_cat(char *args);
int cmd_echo(char *args);
int cmd_env(char *args);
int cmd_help(char *args);
int cmd_ls(char *args);
int cmd_set(char *args);
int cmd_unset(char *args);
int cmd_login(char *args);
int cmd_su(char *args);
char* get_cmd(char *cmd, int len);
int parse_cmd();

FILE *fd;
int ivar;
int logged;
int admin;


struct cmd_handler commands[NCMD] = {
  { "exit", &cmd_exit },
  { "cat", &cmd_cat },
  { "echo", &cmd_echo },
  { "env", &cmd_env },
  { "help", &cmd_help },
  { "ls", &cmd_ls },
  { "set", &cmd_set },
  { "unset", &cmd_unset },
  { "login", &cmd_login },
  { "su", &cmd_su }
};

struct var vars[NVAR];

const char HEADER[] =
"/===========================================================================\\\n"
"/===========================================================================\\\n"
"|                        Cerberus Gate Kepper                               |\n"
"+===========================================================================+\n"
"/===========================================================================\\\n";

const char ACCESS_DENIED[] = "ACCESS DENIED\n";
const char ACCESS_GRANTED[] = "ACCESS GRANTED\n";
const char WELCOME[] = "Welcome Neo...\n";

const useconds_t delay_per_char = 10000;

void text_animation(const char *txt) {
  while (*txt) {
    putc(*txt++, fd);
    fflush(fd);
    usleep(delay_per_char);
  }
}

void verify_animation(unsigned n_cycles) {
  const char states[] = {'/', '-', '\\', '|'};
  for (unsigned i = 0; i < n_cycles; i++) {
    for (int j = 0; j < 3; j++) {
      for (int s = 0; s < sizeof(states); s++) {
        putc(states[s], fd);
        putc('\b', fd);
        fflush(fd);
        usleep(delay_per_char * 10);
      }
    }
    putc('.', fd);
  }
}



char* get_cmd(char *cmd, int len) {
  fprintf(fd, "%s",  admin ? "\033[0;32m(admin) ~" : "$");
  return fgets(cmd, len, fd);
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
  return INVALID;
}

void init() {
  int i;
  
  admin = 0;
  logged = 0;
  ivar = 0;
  for (i = 0; i < NVAR; i++) {
    vars[i].name[0] = 0;
    vars[i].value[0] = 0;
  }
  char supersu[] = "ADMIN";
  strncpy(vars[0].name, supersu, sizeof(supersu));
  ivar++;
}

int is_logged() {
  return (logged == 1); 
}

int main_queue() {
  char cmd[STRSZ];
  int ret;
  text_animation(HEADER);
  for (;;) {
    if (get_cmd(cmd, STRSZ) == NULL)
      break;
    ret = parse_cmd(cmd);
    switch (ret) {
    case OK:
      continue;
    case UNAUTH:
      fprintf(fd, "[!] A logged user is required\n");
      continue;
    case DENIED:
      goto _denied;
    case EXIT:
      goto _exit;
    default:
      fprintf(fd, "[x] Invalid command\n");
    }
  }
_denied:
  fprintf(fd, "\033[0;31m[!] Intruder detected...\n");
  return OK;

_exit:
  fprintf(fd, "[-] See you soon :)\n");
  return OK;
}

int cmd_su(char *argv) {
  if (!is_logged())  return UNAUTH;

  text_animation(" ~> Getting privileges...");
  verify_animation(3);
  for (int i = 0; i < ivar; i++) {
    if (strncmp(vars[i].name, "ADMIN", sizeof("ADMIN")) == 0 &&
      strncmp(vars[i].value, "true", sizeof("true")) == 0)  {
      admin = 1;
      fprintf(fd, "\n");
      break;
    }
  }
  return OK;
}

int cmd_login(char *args) {
  char *ptr, *user = NULL, *pass = NULL;

  if ((ptr = strchr(args, ' ')) == NULL) {
    fprintf(fd, "usage: login <username> <password>\n");
    return OK;
  } else {
    		*ptr = '\0';
  }
  user = args;
	pass = ptr+1;

  if (!args) {
    text_animation(ACCESS_DENIED);
    return DENIED;
  }

  text_animation(" ~> Verifying...");
  verify_animation(3);
  if (strncmp(user, "an0nym0us", sizeof(*user)) != 0) {
    text_animation(ACCESS_DENIED);
    return DENIED;
  }
    
  if (strncmp(pass, "Intrud3r", sizeof(*pass)) != 0) {
    text_animation(ACCESS_DENIED);
    return DENIED;
  }

  text_animation(ACCESS_GRANTED);
  text_animation(WELCOME);
  logged = 1;

  return OK;
}

int cmd_cat(char *args) {
  if (!is_logged())  return UNAUTH;

  FILE *flag;
  char buf[STRSZ];

  if (!is_logged())  return UNAUTH;
  
  if (!args)
    return OK;

  if (!admin) {
    fprintf(fd, "ERROR: permission denied\n");
    return OK;
  }
  
  if ((strncmp(args, "./", 2) == 0)) {
     fprintf(fd, "ERROR: permission denied\n");
    return OK;
  }

  if ((strncmp(args, "/", 1) == 0)) {
     fprintf(fd, "ERROR: permission denied\n");
    return OK;
  }

  if ((strncmp(args, "..", 2) == 0)) {
     fprintf(fd, "ERROR: permission denied\n");
    return OK;
  }

  flag = fopen(args, "r");
  if (flag == NULL) {
    fprintf(fd, "ERROR: cannot open file\n");
    return OK;  }
  fgets(buf, STRSZ, flag);
  fclose(flag);
  fprintf(fd, "%s", buf);

  return OK;
}

int cmd_echo(char *args) {
  if (!is_logged())  return UNAUTH;
  
  if (args) fprintf(fd, "%s\n", args);
  return OK;
}

int cmd_env(char *args) {
  if (!is_logged())  return UNAUTH;
  
  int i;

  for (i = 0; i < ivar; i++) {
    if (vars[i].name[0] == '\0') continue;
    fprintf(fd, "%s: %s\n", vars[i].name, vars[i].value);
  }
  return OK;
}

int cmd_exit(char *args) {
  return EXIT;
}

int cmd_help(char *args) {
  fprintf(fd, "Available commands:\n");
  fprintf(fd, "* cat echo env exit help ls set unset login\n");
  return OK;
}

int cmd_ls(char *args) {
  if (!is_logged()) return UNAUTH;  

  DIR *dir;
  struct dirent *d;
  
  dir = opendir(".");
  if (dir == NULL) {
    fprintf(fd, "ERROR: cannot open dir");
    return OK;
  }
  while ((d = readdir(dir)) != NULL) {
    if (strncmp(d->d_name, ".flag.txt", sizeof(d->d_name)) != 0) {
      fprintf(fd, "%s\n", d->d_name);
    }
  }
  closedir(dir);
  return OK;
}

int cmd_set(char *args) {
  if (!is_logged()) return UNAUTH;  

  char *ptr, *name = NULL, *value = NULL;
  int dup = 0, i;

  if (!args) {
    fprintf(fd, "usage: set <var> <value>\n");
    return OK;
  }

  if ((ptr = strchr(args, ' ')) == NULL) {
    fprintf(fd, "usage: set <var> <value>\n");
    return OK;
  } else {
    *ptr = '\0';
  }

  name = args;
  value = ptr+1;

  for (i = 0; i < ivar; i++) {
    if (strcmp(vars[i].name, name) == 0) {
      strncpy(vars[i].value, value, STRSZ);
      dup = 1;
      break;
    }
  }

  if (!dup) {
    if (ivar > NVAR-1) {
      fprintf(fd, "ERROR: out of memory\n");
      return OK;
    }
    strncpy(vars[ivar].name, name, STRSZ);
    strncpy(vars[ivar].value, value, STRSZ);
    ivar++;
  }

  return OK;
}

int cmd_unset(char *args) {
  if (!is_logged()) return UNAUTH;  

  int i;

  if (!args) {
    fprintf(fd, "usage: unset <var>\n");
    return OK;
  }

  for (i = 0; i < ivar; i++) {
    if (strcmp(vars[i].name, args) == 0) {
      vars[i].name[0] = '\0';
      break;
    }
  }
  return OK;
}

int main(int argc, char *argv[]) {
  struct sockaddr_in srvaddr, cliaddr;
  int client, sock, pid;
  
  init();
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
      if ((fd = fdopen(client, "r+")) == NULL) {
        perror("fdopen");
        close(client);
        break;
      }
      close(sock);
      main_queue();
      fclose(fd);
      close(client);
      break;
    }
  }
  if (pid != 0) close(sock);
  return 0;
}
