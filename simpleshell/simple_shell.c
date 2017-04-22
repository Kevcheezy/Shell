#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#define MAX_TOKEN_LENGTH 50
#define MAX_TOKEN_COUNT 100
#define MAX_LINE_LENGTH 512

// Simple implementation for Shell command
// Assume all arguments are seperated by space
// Erros are ignored when executing fgets(), fork(), and waitpid(). 


void runcommand(char* command, char** args, int in, int out, int pipe, char input[64], char output[64]) {
  pid_t pid = fork();
  if(pid) { // parent
    	waitpid(pid, NULL, 0);
  } else { // child'
      
      if (in){ // if '<'' is detected in input
        int fd0;
        if (( fd0 = open(input, O_RDONLY, 0)) < 0){
          perror("Couldn't open input file");
          exit(0);
        }
        dup2(fd0, 0);
        close(fd0);
      }
      
    	execvp(command, args);
      perror("execvp");
      exit(1);
  }
}

int main(){
    char line[MAX_LINE_LENGTH];
    //printf("shell: "); 
    while(fgets(line, MAX_LINE_LENGTH, stdin)) {
    	// Build the command and arguments, using execv conventions.
    	line[strlen(line)-1] = '\0'; // get rid of the new line
    	char* command = NULL;
    	char* arguments[MAX_TOKEN_COUNT];
    	int argument_count = 0;
    	char* token = strtok(line, " ");
      int in = 0;
      int out = 0;
      int pipe = 0;
      char input[64], output[64];

    	while(token) {
          if(strstr(token, ">")) out = 1;; //implement ">" here
          if(strstr(token, "<")) in = 1; //implement "<" here
          if(strstr(token, "|")) pipe = 1; //implement "|" here
      		if(!command) command = token;
      		arguments[argument_count] = token;
	      	argument_count++;
      		token = strtok(NULL, " ");
    	}
    	arguments[argument_count] = NULL;
    	if(argument_count>0){
    		if (strcmp(arguments[0], "exit") == 0) exit(0);
        for(int i = 0; arguments[i]!='\0';i++){

          if(strcmp(arguments[i],"<") == 0){
            arguments[i]=NULL;
            strcpy(input, arguments[i+1]);
            in=2;
          }
          if(strcmp(arguments[i],">") == 0){
            arguments[i]=NULL;
            strcpy(output, arguments[i+1]);
            out=2;
          }
        }
        runcommand(command, arguments, in, out, pipe, input, output);
        //printf("input is : %s \n", input);
        //printf("output is : %s \n", output);
    	}
    }
    return 0;
}
