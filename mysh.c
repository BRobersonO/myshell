#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <signal.h>

#define DELIMS " \t\r\n\a"
#define BUFFER 1024
#define ARGS_SZ 64


char history[10][BUFFER];
int counter = 0;
int counter2 = 0;
pid_t *pids;
int asize = BUFFER;
//array for PIDS for exterminateall command
void setup_array (void) {
	pids = malloc (asize * sizeof (pid_t));
}
//reallocate array if we run out of space
void reall_array (void) {
	asize += BUFFER;
	pids = realloc (pids, asize * sizeof(pid_t));
}
//reads the line entered into prompt
char *readline (void) {
	char *line = NULL;
	ssize_t buffer = 0;
	getline (&line, &buffer, stdin);


	//put into history for the lastcommands command
	for (int i=9;i>0;i--) {
		strcpy(history[i],history[i-1]);
	}
	strcpy(history[0],line);
	counter ++;
	return line;
}
//tokenize the line entered into prompt
char **parse (char *line) {
	int size = ARGS_SZ;
	int pos = 0;
	char **args = malloc (size * sizeof(char*));
	char *token;

	token = strtok (line, DELIMS);
	while (token != NULL) {
		args[pos] = token;
		pos++;
		if (pos >= size) {
			size += ARGS_SZ;
			args = realloc (args, size * sizeof(char*));
		}
		token = strtok(NULL, DELIMS);
	}
	args[pos] = NULL;

	return args;
}
//used for the lastcommands command
void showhistory(void) {
	for (int i=counter;i>0;i--) {
		if(history[i]!=NULL) {
			printf("%s",history[i]);
		}
		else break;
	}
}
//used for the lastcommands -c command
void clearhistory(void) {
	for (int i=counter;i>=0;i--) {
		strcpy(history[i],"");
	}
	counter = 0;
}
//execute built-in commands
int execute (char **args) {
	if(args[0] == NULL)
		return 1;
	if (args[0] != NULL && !strcmp(args[0],"whereami")) {
		system("pwd");
		return 1;
	}
	if (args[0] != NULL && !strcmp(args[0],"changedir")) {
		chdir(args[1]);
		return 1;
	}
	if (args[0] != NULL && !strcmp(args[0],"lastcommands")) {
		if (args[1] != NULL && !strcmp(args[1],"-c")) {
			clearhistory();
			return 1;
		}
		showhistory();
		return 1;
	}
	if (args[0] != NULL && !strcmp(args[0],"run")) {
		pid_t pid = fork();
		int status;
		if(pid<0) {
			perror("fork failed.");
			exit(1);
		}
		else if (pid == 0) {
		//child
			execv(args[1],args);
		}
		if (counter2 >= asize)
			reall_array();
		pids[counter2] = pid;
		counter2 ++;

		waitpid(pid, &status, 0);
		return 1;
	}
	if (args[0] != NULL && !strcmp(args[0],"background")) {
		pid_t pid = fork();
		if(pid<0) {
			perror("fork failed.\n");
			exit(1);
		}
		else if (pid == 0) {
		//child
			execv(args[1],args);
		}
		if (counter2 >= asize)
			reall_array();
		pids[counter2] = pid;
		counter2 ++;

		printf("The Process ID of the current process is %d. \n", pid);
		return 1;
	}
	if (args[0] != NULL && !strcmp(args[0],"exterminate")) {
		char* eend;
		pid_t epid;
		if (args[1] != NULL)
			epid = strtol (args[1], &eend, 10);
		if (epid > 0 && !kill (epid, 0)) {
			kill(epid, SIGKILL);
			printf("Process %d has been terminated.\n",epid);
		}
		else
			perror("there was an error");
		for (int i=0; i<counter2; i++) {
			if (pids[i] == epid)
				pids[i] = 0;
		}
		return 1;
	}
	if (args[0] != NULL && !strcmp(args[0],"repeat")) {
		long num;
		char * end;
		if (args[1] != NULL)
			num = strtol (args[1], &end, 10);
		if (num > 0) {
			for (long i=0; i<num; i++) {
				pid_t rpid = fork();
				if(rpid<0) {
					perror("fork failed.\n");
					exit(1);
				}
				else if (rpid == 0) {
				//child
					execv(args[2],args);
				}
				if (counter2 >= asize)
					reall_array();
				pids[counter2] = rpid;
				counter2 ++;

				printf("The Process ID of process %ld is %d.\n",i+1, rpid);
			}
		}
		else {
			perror("Must have a positive number.\n");
			return 1;
		}
		return 1;
	}
	if (args[0] != NULL && !strcmp(args[0],"exterminateall")) {
		for (int i=0; i<counter2; i++) {
			if (pids[i] > 0 && !kill (pids[i], 0)) {
				kill(pids[i], SIGKILL);
				printf("Process %d has been terminated.\n",pids[i]);
				pids[i] = 0;
			}
		}
		counter2 = 0;
		return 1;
	}
	if (args[0] != NULL && !strcmp(args[0],"quit")) {
		return 0;
	}
}
/* MAIN LOOP */
int main (int argc, char **argcv) {

	char * line ;
	char ** args;
	int status;
	setup_array();
	const char* CLEAR_SCREEN_ANSI = " \e[1;1H\e[2J";
	write (STDOUT_FILENO, CLEAR_SCREEN_ANSI, 12);

	do {
	printf("%s","# ");
	line = readline();//read command
	args = parse(line);//parse command
	status = execute(args);//exe comand

	free(args);
	}while(status);
	free(pids);
return 0;
}
