#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>

const int BUFSIZE = 1024;
const int ARG_BUFSIZE = 1024;

char *read_line(){
	int pos = 0;
	char *buffer = malloc(sizeof(char) * BUFSIZE);
	
	int c;
	while (1){
		c = getchar();

		if (pos == 0 && c == EOF) {
			printf("Bye\n");
			exit(0);
		} else if (c == EOF || c == '\n' || pos == BUFSIZE-1) {
			buffer[pos] = '\0';
			return buffer;
		} 

		buffer[pos] = c;
		pos++;
	}
}

char **split_line(char *line) {
	int pos = 0;
	char *sep = " ";
	char **args = malloc(ARG_BUFSIZE * sizeof(double));
	char *arg, *line_copy;
	//strncpy(line_copy, line, strlen(line));

	arg = strtok(line, sep);
	while (arg != NULL) {
		int len = strlen(arg);
		char *arg_cpy = malloc((len + 1) * sizeof(char));
		strncpy(arg_cpy, arg, len+1);
		arg_cpy[len] = '\0';

		args[pos] = arg_cpy;
		pos++;

		arg = strtok(NULL, sep);
	}
	return args;
}

char *prev_dir, *prev_tmp;
int invoke_command(char **args){
	// nothing
	if (args[0] == NULL) {
		return 1;
	} else if (strcmp(args[0], "cd") == 0){
		if (args[1] == NULL) {
			fprintf(stderr, "usage: cd <DirectoryName>\n");
			return 1;
		} else if (strcmp(args[1], "-") == 0){
			args[1] = prev_dir;
		}
		
		getcwd(prev_tmp, BUFSIZE);
		if (chdir(args[1]) == -1){
			printf("can not change directory to %s\n", args[1]);
		} else {
			memcpy(prev_dir, prev_tmp, BUFSIZE);
		}
		return 1;
	}

	pid_t pid;
	pid = fork();

	// if fork failed
	if (pid == -1){
		fprintf(stderr, "fork failed!\n");
		exit(EXIT_FAILURE);
	}

	// child
	int child_stat = 0;
	int err = 0;
	if (pid == 0) {
		err = execvp(args[0], args);
		if (err < 0){
			printf("%s: %s\n", args[0], strerror(errno));
			exit(errno);
		}
	//parent
	} else {
		int result = wait(&child_stat);
		if (result < 0) {
			fprintf(stderr, "Wait Error!\n");
			exit(EXIT_FAILURE);
		}
	}
	return 1;
}

int main() {
	printf("Welcome to my shell!\n");

	int stat = 1;
	char* line;
	char** args;
	prev_dir = malloc(BUFSIZE * sizeof(char));
	prev_tmp = malloc(BUFSIZE * sizeof(char));
	do {
		printf("$ ");
		line = read_line();
		args = split_line(line);
		stat = invoke_command(args);
	} while(stat);
}