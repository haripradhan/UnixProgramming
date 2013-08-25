///Name: Hari Pradhan
///Date: 06/30/2013
///Purpose: This program will provide a shell that is capable of executing other programs. 
///It reads a command from command line and execute the command.
///Steps:
///1. Displays the promot "CS590-sh>".
///2. Allows the user to enter a command.
///3. Parses the command entered by the user and fork/exec a process to execute the command.
///4. Quit the shell if user enters "exit" command.
///5. SIGKILL -9 kills the shell. Parent ignores SIGINT, SIGQUIT signals while child takes default action.

#include <sys/types.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>

pid_t pid;

/// <summary>
/// Signal Handler for child process
/// </summary>
/// <param name="signum">Signal number</param>
void sigHandler(int signum){
	switch(signum){
	case SIGINT:
		signal(SIGINT,SIG_DFL);
		break;
	case SIGQUIT:
		signal(SIGQUIT,SIG_DFL);
		if(kill(pid,SIGQUIT) == -1){
			perror("Kill Error");
		}
		break;
	default:
		break;		
	}
}

/// <summary>
/// Signal Handler for parent process
/// </summary>
/// <param name="signum">Signal number</param>
void sigParentHandler(int signum){
	printf("\nCS590-sh> ");
	fflush(stdout);
}

/// <summary>
/// Counts the number of arguments
/// </summary>
/// <param name="str">command line string</param>
int countArguments(char*  str){
	char* arg;
	int count =0;
	arg = strtok(str," ");
	while(arg != NULL){
		count++;
		arg = strtok(NULL," ");
	}
	return count;
}

/// <summary>
/// Parse the command line string
/// </summary>
/// <param name="str">command line string</param>
/// <param name="parsedArgv">Array of argument strings</param>
void parseCommandLine(char* str, char* parsedArgv[]){
	char* arg;
	int i =0;
	//Reference for strtok to parse command line string : man 3p page of strtok
	arg = strtok(str," ");
	while(arg != NULL){
		parsedArgv[i]=(char*)malloc((sizeof(char)*strlen(arg)));
		strcpy(parsedArgv[i],arg);
		arg = strtok(NULL," ");
		i++;
	}	
	parsedArgv[i] =NULL;
}

int main(){
	char commandStr[50];
	int status;
	int num_args =0;
	char* tempStr = (char*)malloc(sizeof(commandStr));
	signal(SIGINT, sigParentHandler);
	signal(SIGQUIT, sigParentHandler);
	signal(SIGTSTP, sigParentHandler);
	printf("CS590-sh> ");
	while(1){
		fgets(commandStr,50,stdin);		
		if(commandStr[0] == '\n'){
			printf("CS590-sh> ");
			continue;
		}
		if(commandStr!= NULL){
			commandStr[strlen(commandStr)-1]=0;
			strcpy(tempStr,commandStr);
			num_args = countArguments(tempStr);

			char** parsedArgv = (char**) malloc((sizeof(char*) * (num_args+1)));
			parseCommandLine(commandStr,parsedArgv);

			if(strcmp(parsedArgv[0],"exit") == 0){
				exit(0);
			}
			//Fork to spawn child process for execvp
			if ((pid = fork()) == -1){
				perror("fork() error");
			}
			else if (pid == 0){ //Child process
				signal(SIGINT, sigHandler);
				signal(SIGQUIT, sigHandler);
				signal(SIGTSTP,SIG_IGN); 
				//Execute command using execvp function
				if(execvp(parsedArgv[0],parsedArgv) <0){
					perror("Exec error");
					exit(0);
				}
			}
			else{	//Parent process
				wait(&status);
				//Check the normal termination of the child process i.e. execvp
				if(WIFEXITED(status)){
					printf("CS590-sh> ");
				}
				signal(SIGINT,sigParentHandler);
				signal(SIGQUIT,sigParentHandler);
				signal(SIGTSTP, sigParentHandler);

			}
			free(parsedArgv);
		}
		strcpy(commandStr,"");
	}

	return 0;
}


