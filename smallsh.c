/*
Chido Nguyen
931506965
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>


//Parent ctrl-c handler//
void catchSIGINT(int signo){
	int x;
	//just a dummy catch , child processes will gut this anyways//
}


/*
nonBuiltParse()
-We take the argument list and check for special characters like < > and final & symbols
0 if <
1 if >
2 if both < > 
*/
void nonBuiltParse(char** arr, char** argv, int arg_count){
	int j;
	int caseNumber = -1;
	int bground = -1;
	int fname1=-1;
	int fname2=-2;
	int sourceFD, targetFD,result;
	
	// Check for file redirection symbols 
	for(j = 0 ; j < arg_count; j++){
		if(strcmp(arr[j], "<") == 0){
			caseNumber++;
			fname1= j+1;
			
		}
		else if (strcmp(arr[j], ">") == 0){
			caseNumber++;
			caseNumber++;
			fname2 = j+1;
		}
	}
	//open appropriate File descriptors // 
	//For each case either 1 of either < or > is found , both or none//
	//when found will either open for read or write and swap the with stdin/out via dup2 as needed//
	//will also port over other arguments in command line prior to the redirection//
	if(caseNumber == 0){
		
		sourceFD = open(arr[fname1], O_RDONLY);
		if(sourceFD == -1){
			perror("File can't be opened");
			exit(1);
		}
		result = dup2(sourceFD, 0);
		if(result == -1) {
			perror("Source in failed");
			exit(2);
		}
		
		for(j = 0 ; j < fname1 -1; j++){
			argv[j]=arr[j];
		}
		argv[fname1-1] = NULL;
	}
	else if (caseNumber == 1){
		targetFD = open(arr[fname2],O_WRONLY | O_CREAT | O_TRUNC , 0644);
		if(targetFD == -1){
			perror("File can't be opened or failed to be created");
			exit(1);
		}
		
		result = dup2(targetFD,1);
		if(result == -1) {
			perror("Out target failed");
			exit(2);
		}
		
		for(j = 0 ; j < fname2 -1; j++){
			argv[j]=arr[j];
		}
		argv[fname2-1] = NULL;
	}
	else if (caseNumber == 2){
		sourceFD = open(arr[fname1], O_RDONLY);
		if(sourceFD == -1){
			perror("File can't be opened");
			exit(1);
		}
		targetFD = open(arr[fname2],O_WRONLY | O_CREAT | O_TRUNC , 0644);
		if(targetFD == -1){
			perror("File can't be opened or failed to be created");
			exit(1);
		}
		result = dup2(targetFD,1);
		if(result == -1) {
			perror("Out target failed");
			exit(2);
		}
		result = dup2(sourceFD, 0);
		if(result == -1) {
			perror("Source in failed");
			exit(2);
		}
		
		if(fname1 < fname2){
			for(j = 0 ; j < fname1 -1; j++){
				argv[j]=arr[j];
			}
			argv[fname1-1] = NULL;
		}
		else{
			for(j = 0 ; j < fname2 -1; j++){
				argv[j]=arr[j];
			}
			argv[fname2-1] = NULL;
		}
	}
	else{
		for(j = 0 ; j < arg_count; j++){
			argv[j] = arr[j];
		}
		argv[arg_count] = NULL;
	}
}



/*
not_my_problem
-Function handles dumping all non built-in commands to fork/exec for the "real" shell to handle.
Input: Sends the array of arguments + the argument count to function
Output: Void function with no return, except things will be dumped onto terminal via stdout and what not.
*/
void not_my_problem(char** arr, int arg_count){
	pid_t spawnPID = -5;
	int childExitStatus= -5;
	spawnPID = fork();
	
		if(spawnPID == -1){
			perror("You goof'd\n");
			exit(1);
		}
		else if (spawnPID == 0){
			char* argv[512];
			memset(&argv, 0 , sizeof(argv));
			nonBuiltParse(arr,argv,arg_count);
			execvp(*argv,argv);
			exit(0);
		}
		waitpid(spawnPID, &childExitStatus, 0);
		int exitStatus = WEXITSTATUS(childExitStatus);
		printf("Exit Status: %i\n",exitStatus);
}


/*
arg_Parser
Parameters: Takes an array of char which stored user input and char* array to parse commands out to be stored, and argument counter as x
Output: void function , but arguments Array should be updated directly 
*/
void arg_Parser(char* cmdArr, char** args, int* x){
	

	char *token; // "holder" for strtok
	//First word look up//
	token = strtok(cmdArr, " ");
	// process rest of string input //
	while(token != NULL){
		args[(*x)] = token;
		(*x)++;
		token = strtok(NULL, " ");
	}
}

/*
cd_handler
Function manages chdir to either ~ or to specified path
input: array arguments 
output: no return
*/
void cd_handler(char** arr){
	char *directory;
	//checks to see if cd came with argument if it does chdir to that argument
	if( arr[1] != NULL){
		directory = arr[1];
		int ret = chdir(directory);
		//printf("%i\n",ret); // check status of opening dir
	}
	else {
		// first attempt was using ~ but as link informed ~ is expanded based on shell. My shell doesn't support this so work around is to getenv and send it home
		//https://stackoverflow.com/questions/9493234/chdir-to-home-directory
		int ret;
		ret = chdir(getenv("HOME"));
		//printf("%i\n", ret);
	}
}

/*
arg_Process
-Determines what the first word of the command is to operate properly
Input: Takes arguments array (char* arr[])
Output: Void or maybe an Int depending on implementation inside function or returns
*/
void arg_Process(char** arr, int arg_count){
	char * command = arr[0];
	if(strcmp( command, "exit") == 0){
		exit(0);
	}
	else if(strcmp(command, "cd") == 0){
		cd_handler(arr);
	}
	else if (strcmp( command, "status") == 0){
		exit(0);
	}
	else
		not_my_problem(arr, arg_count);
}


/*
money_maker 
-Checks for any $$ and gets the shell PID
Input: array of our parsed arguments and # of arguments
Output: no returns, just print to screen of PID 
*/
int money_maker(char** arr, int x){
	int y;
	for(y = 0; y < x; y++){
		if(strcmp( arr[y], "$$") ==0){
			printf("%i\n", getpid());
			return 0;
		}
	}
	return 1;
}


/*
loop_sh
-our shell emulator that loops until exit is called
*/
void loop_sh(){
	//user command input and arguments storage variables
	char cmd_length[2048];
	char* arguments[512] = {NULL};
	char* pos;
	memset(cmd_length, '\0', sizeof(cmd_length));
	int arg_count = 0;
	
	while(1){
		//CLEARS array 
		memset(cmd_length, '\0', sizeof(cmd_length));
		memset(&arguments, 0, sizeof(arguments));
		//: prompt user on console
		printf(": ");
		fflush(stdout);
		
		//userinput  + clearing final newline character
		fgets(cmd_length , 2048 , stdin);//https://stackoverflow.com/questions/2693776/removing-trailing-newline-character-from-fgets-input//
		// Removing trailing new line in user input
		if((pos = strchr (cmd_length, '\n')) != NULL)
			*pos = '\0';
		
		//do not parse if user has empty input or comment line //
		if(cmd_length[0] != 0 && cmd_length[0] != '#'){
			arg_Parser(cmd_length, arguments, &arg_count); //parse command into args
			if(money_maker(arguments, arg_count)){
				arg_Process(arguments, arg_count);
			}
		}
		
		arg_count = 0;
	}
}

int main(){
	struct sigaction SIGINT_action = {0};
	
	SIGINT_action.sa_handler=catchSIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags=0;
	
	//Registers sigint to siginit action struct //
	sigaction(SIGINT, &SIGINT_action, NULL);
	
	loop_sh();
	return 0;
}