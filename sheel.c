/*
	Sheel is written by following this tutorial:
	https://brennan.io/2015/01/16/write-a-shell-in-c/
	Note: Implementation is not finished
*/

#include <sys/wait.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#define SHEEL_BUFFER_SIZE 1024
#define SHEEL_TOKEN_SIZE 64
#define SHEEL_TOKEN_DELIMITER " \t\r\n\a"
#define HISTORY_SIZE 10

int sheel_loop(void);
char *sheel_read_line(void);
char **sheel_split_args(char* line);
int sheel_run(char **args);
void sheel_save_history(char **args);
void show_history();

int main(int argc, char **argv) {
	return sheel_loop();
}

char** hist;
int cur_history;

int sheel_loop(void) {

	char *line;
	char **args;
	int result;

	hist = malloc(HISTORY_SIZE * sizeof(char*));
	cur_history = 0;

	do {
		printf("> ");
		line = sheel_read_line();
		args = sheel_split_args(line);
		result = sheel_run(args);
		sheel_save_history(args);

		free(line); 
		free(args);

	} while (result);

	return 1;
}

void sheel_save_history(char **args) {
	hist[cur_history] = strdup(args[0]);
	cur_history++;
}

//todo: fix error
void show_history() {

	int i = 0;

	for (int i = 0; i < HISTORY_SIZE; ++i){
		printf("$ %s\n", hist[i]);
	}

}

char *sheel_read_line(void) {

	int pos = 0;
	char *buffer = malloc(sizeof(char) * SHEEL_BUFFER_SIZE);
	int c;

	if (!buffer) {
		fprintf(stderr, "sheel malloc error\n");
		exit(EXIT_FAILURE);
	}

	while (1) {
		// read a character
		c = getchar();

		// If we hit EOF, replace it with a null character and return.
		if (c == EOF || c == '\n') {
			buffer[pos] = '\0';
			return buffer;
		} else {
			buffer[pos] = c;
		}

		pos++;
	}
}

char **sheel_split_args(char* line) {

	int pos = 0;
	char **tokens = malloc(SHEEL_TOKEN_SIZE * sizeof(char*));
	char *token;

	if (!tokens) {
		fprintf(stderr, "sheel realloc error\n");
		exit(EXIT_FAILURE);
	}

	token = strtok(line, SHEEL_TOKEN_DELIMITER);

	while (token != NULL) {
		tokens[pos] = token;
		pos++;
		token = strtok(NULL, SHEEL_TOKEN_DELIMITER);
	}

	tokens[pos] = NULL;
	return tokens;
}


int sheel_run(char **args) {

	pid_t pid;
	int status;

	if(strcmp(args[0], "exit") == 0) {
		printf("exiting sheel!\n");
		return 0;
	}

	if(strcmp(args[0], "history") == 0) {
		show_history();
		return 1;
	}

	pid = fork();

	if (pid == 0) {
		// child process
		if (execvp(args[0], args) == -1) {
			perror("arg error");
		}
		exit(EXIT_FAILURE);
	} else if (pid < 0) {
		perror("fork error");
	} else {
		// wait for parent process
		wait(NULL);
	}

	return 1;
}