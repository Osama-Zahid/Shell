#include<stdio.h> 
#include<string.h> 
#include<stdlib.h> 
#include<unistd.h> 
#include<sys/types.h> 
#include<sys/wait.h> 
#include <fcntl.h>
#include<readline/readline.h> 
#include<readline/history.h> 

int stringlength(char* s){
	int count=0;
	while(s[count]!='\0'){
		count++;	
	}
	return count;
}
int stringcompare(char* s, char* d){
	if(stringlength(s)==stringlength(d)){
		for(int i=0;d[i]!='\0';i++){		
			if(s[i]!=d[i]){
				return 0;
			}
		}
		return 1;
	}
	return 0;
}
void initializeShell()
{
    char* username = getenv("USER");    //return the current User of OS
    printf("Initializing Shell......"); 
    printf("\nUSER is: @%s", username); 
    printf("\n"); 
    sleep(1); 
}
void printCurrentDirectory() 
{ 
    char cwd[1024];                    //return current working directory
    getcwd(cwd, sizeof(cwd)); 
    printf("\nCurrent Directory: %s", cwd); 
}
int Input(char* str) 
{ 
    char* buffer; 
  	
    //printf("Input String");
    buffer = readline(":~$  "); 
    if (strlen(buffer) != 0) 
    { 
     	// add_history(buffer);   //store every line in the history
        strcpy(str, buffer); 
        return 0; 
    } 
    else 
    { 
        return 1; 
    } 
} 
// function for finding pipe 
int parsePipe(char* str, char** strpiped) 
{ 
	//printf("InparesedPipe");
    int i; 
    for (i = 0; i < 2; i++) 
    { 
        strpiped[i] = strsep(&str, "|"); 
        if (strpiped[i] == NULL) 
            break; 
    } 
  
    if (strpiped[1] == NULL) 
        return 0; // returns zero if no pipe is found. 
    else
    { 
        return 1; 
    } 
} 

// Function to execute builtin commands 
int ownCmdHandler(char** parsed) 
{ 
    int NoOfOwnCmds = 3, i, switchCommand = 0; 
    char* ListOfOwnCmds[NoOfOwnCmds]; 
    char* username; 
  
    ListOfOwnCmds[0] = "exit"; 
    ListOfOwnCmds[1] = "cd"; 
    ListOfOwnCmds[2] = "help"; 

  
    for (i = 0; i < NoOfOwnCmds; i++) { 
        if (strcmp(parsed[0], ListOfOwnCmds[i]) == 0) { 
            switchCommand = i + 1; 
            break; 
        } 
    } 
	if(switchCommand==1)
	{
	        printf("\nEXIT\n"); 
        	exit(0); 	
	}
  	else if(switchCommand==2)
	{
	    char cwd[1024];                    //return current working directory
	    getcwd(cwd, sizeof(cwd)); 
	   chdir(parsed[1]);   //cd command 
	    char cwd1[1024];                    //return current working directory
	    getcwd(cwd1, sizeof(cwd)); 
	if(stringcompare(cwd,cwd1))
	{
		perror("no directory found"); 
	}
	}

    return 0; 
} 
// function for parsing command words 
void parseSpace(char* str, char** parsed) 
{ 
    int i; 
  
    for (i = 0; i < 100; i++) 
    { 
        parsed[i] = strsep(&str, " "); 
  
        if (parsed[i] == NULL) 
            break; 
        if (strlen(parsed[i]) == 0) 
            i--; 
    } 
}
void parseOnsort(char* str, char** parsed) 
{ 
    int i; 
  
    for (i = 0; i < 100; i++) 
    { 
        parsed[i] = strsep(&str, " "); 
  
        if (parsed[i] == NULL) 
            break; 
       
    } 
	parsed[i-1]=NULL; 
} 
 
int processString(char* str, char** parsedString, char** parsedpipeString) 
{ 
  
    char* strpiped[2]; 
    int piped = 0; 
  
    piped = parsePipe(str, strpiped); 
  
    if (piped) { 
        parseSpace(strpiped[0], parsedString); 
        parseSpace(strpiped[1], parsedpipeString); 
  
    } else { 
  
        parseSpace(str, parsedString); 
    } 
  
    if (ownCmdHandler(parsedString)) 
        return 0; 
    else
        return 1 + piped; 
} 
void SystemCommands(char** parsed) 
{ 
    // Forking a child 
    pid_t pid = fork();  
  
    if (pid == -1) 
    { 
        printf("\nFailed forking child.."); 
        return; 
    } 
    else if (pid == 0) 
    { 
        if (execvp(parsed[0], parsed) < 0) 
        { 
          //  printf("\nCould not execute command......."); 
        } 
        exit(0); 
    }
    else 
    { 
        // waiting for child to terminate 
        wait(NULL);  
        return; 
    } 
} 
  
// Function where the piped system commands is executed 
void SystemCommandsPiped(char** parsed, char** parsedpipe) 
{ 
  int fds[2]; 
  pipe(fds);
  
  if (fork() == 0) {
    dup2(fds[0], STDIN_FILENO);
    close(fds[1]);
    close(fds[0]);

    if (fork() == 0) {
 
        dup2(fds[1], STDOUT_FILENO);
        close(fds[0]);
        close(fds[1]);
        execvp(parsed[0], parsed);
    }
    wait(NULL);
    execvp(parsedpipe[0], parsedpipe);
    }
    close(fds[1]);
    close(fds[0]);
    wait(NULL);
} 
int parseRedirection(char* str, char** commands) 
{ 
	int i; 
   for (i = 0; i < 2; i++)
   { 
        commands[i] = strsep(&str, ">"); 
        if (commands[i] == NULL) 
            break; 
    } 
  
    if (commands[1] == NULL) 
        return 0;
    else 
    { 
        return 1; 
    } 
} 
int checkRedirection(char *str,char** commands)
{
	//printf("In Check REDirect");
	char* checkRedir[2]; //separate both parts on each side of pipe
	char* temp[2];
    	int r = 0; 
	int check=0;
	char cmd[20];
  
   	 r = parseRedirection(str,checkRedir); 
	
//	printf("%s",checkRedir[0]);  
   if (r) 
   { 
        parseOnsort(checkRedir[0], commands); 	
	int j=0,k=0;
	//	printf("%s",*(checkRedir+1));  	
	for(j=0;checkRedir[1][j]!='\0';j++)
	{
		if(checkRedir[1][j]!=' ')
		{
		cmd[k++]=checkRedir[1][j];
		}
	}	  
	cmd[k]='\0';
 	//printf("\n%s",cmd);  	
      
	int out;

	pid_t pid;
	pid=fork();
	if(pid==0)
	{
	
        out = open(cmd, O_WRONLY);
	dup2(out, 1);
		
	close(out);
	execvp(commands[0], commands);
	exit(0);
		
	}
	else
	{
		wait(NULL);
		return 1;
	}
	
  }
	return 0;
}
int main()
{
	char inputString[1000];
	char *commands[100];
	char *commandpipe[100];
	int flag=0;
	initializeShell();  //return current user

	
 	while (1) 
	{ 
	// print shell line 
	printCurrentDirectory();   //print current directory using getcwd()
	// take input 
	if (Input(inputString)) 
	    continue; 
	// process 
	checkRedirection(inputString, commands);
		
	flag = processString(inputString, commands,commandpipe); 
	// execflag returns zero if there is no command 
	// or it is a builtin command, 
	// 1 if it is a simple command 
	// 2 if it is including a pipe. 
  	checkRedirection(inputString, commands);
	// execute 
	if (flag == 1) 
	   SystemCommands(commands); 
  
	if (flag == 2) 
	   SystemCommandsPiped(commands,commandpipe); 
 	
	
	 }
    return 0; 
}
