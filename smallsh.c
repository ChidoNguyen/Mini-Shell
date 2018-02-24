/*
Chido Nguyen
931506965
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>



/*
nonBuiltParse()
-We take the argument list and check for special characters like < > and final & symbols
*/
void nonBuiltParse(){

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
			printf("HI I AM CHILD MY PID : %i\n", getpid());
			exit(1);
		}
		waitpid(spawnPID, &childExitStatus, 0);
		if( WIFEXITED(childExitStatus) != 0)
			printf("normal\n");
		printf("Parent my PID: %i\n", getpid());
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
	//else
		//not_my_problem(arr, arg_count);
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
	loop_sh();
	return 0;
}