#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <error.h>
#define MAX 200
#define SMALL 10
#define DEBUG 1

struct cmd {
	char name[SMALL];
	char opts[SMALL];
	char *args[SMALL];
};

static struct cmd cmd_pipes[SMALL];

void init_pipes(){
	int size = sizeof(cmd_pipes) / sizeof(struct cmd);
	struct cmd *cmd_cur = cmd_pipes;
	int args_size = sizeof(cmd_cur->args) / sizeof(char *);
	for (int i = 0; i < size && cmd_cur->name[0]; i++) {
		cmd_cur->name[0] = 0;
		cmd_cur->opts[0] = 0;
		int j = 0;
		while (j < args_size && cmd_cur->args[j]) {
			free(cmd_cur->args[j]);
			cmd_cur->args[j] = NULL;
		}
		++cmd_cur;
	}
}

char *read_cmds(char *buf, int size)
{
	if (isatty(fileno(stdin)))
		fprintf(stdout, "");
	buf = fgets(buf, size, stdin);
	while (buf && buf[strlen(buf) - 1] == '|'){
		fgets(buf + strlen(buf), size - strlen(buf), stdin);
	}
	// in case, buf size is 0
	if (strlen(buf) > 0)
		buf[strlen(buf)-1] = 0;
	return buf;
}

/* read stdin */
int get_cmds(char *buf, int size)
{
	buf = read_cmds(buf, size);
	if (!buf)
		return -1;
	if (!*buf)
		return 0;
	struct cmd *cmd_cur = cmd_pipes;
	int i = 0;
	while (buf[i]) {
		if (buf[i] == '|') {
			++i;
			continue;
		}
		for (; isspace(buf[i]); ++i);
		int j = i;
		for(; buf[i] && !isspace(buf[i]) && buf[i] != '|'; i++);
		strncpy(cmd_cur->name, buf + j, i - j);
		cmd_cur->name[i - j] = 0;

		for (; isspace(buf[i]); ++i);
		int arg_count = 0;
		while (buf[i] && buf[i] != '|') {
			j = i;
			for (; buf[i] && !isspace(buf[i]) && buf[i] != '|'; i++);
			char *dst = NULL;
			if (buf[j] == '-') {
				++j;
				dst = cmd_cur->opts + strlen(cmd_cur->opts);
			} else {
				cmd_cur->args[arg_count] = malloc(i - j + 1);
				dst = cmd_cur->args[arg_count++];
			}
			strncpy(dst, buf + j, i - j);
			dst[i - j] = 0;
			for (; isspace(buf[i]); ++i);
		}
		++cmd_cur;
	}
	return cmd_cur - cmd_pipes;
}

/* print cmds for test */
static void print_cmds(int cmd_count)
{
	printf("The Input cmds are :\n");
	struct cmd *cmd_cur = cmd_pipes;
	for (; cmd_count > 0; cmd_count--, cmd_cur++) {
		printf("%s ", cmd_cur->name); 
		printf((strlen(cmd_cur->opts) > 0) ? "-%s ": "%s ", cmd_cur->opts);
		int i = 0;
		while (cmd_cur->args[i])
			printf("%s ", cmd_cur->args[i++]);
		printf("\n");
	}
}

int run_cmds(const char *buf)
{
	return 0;
}

static void print_error()
{
}


int main(int argc, char *argv[])
{
	char buf[MAX] = {0};
	int count = 0;
	while ((count = get_cmds(buf, MAX)) > 0) {
#if DEBUG == 1
		print_cmds(count);
#endif
		run_cmds(buf);
		init_pipes();
	}
}

