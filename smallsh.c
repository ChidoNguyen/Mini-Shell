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
int TSPT_GLOBAL;

struct status_Pres {
	int PID;
	int childExitStat;
};
struct status_Pres child_HX;

//Parent ctrl-c handler//
void catchSIGINT(int signo){
	char* Message = "terminated by signal 2\n";
	write(STDOUT_FILENO, Message, 23);
	
}

void catchSIGTSTP(int signo){
	if(TSPT_GLOBAL){
		TSPT_GLOBAL = 0;
		char* sms = "\nEntering foreground-only mode (& is now ignored)\n";
		write(STDOUT_FILENO, sms, 50);
	}
	else{
		TSPT_GLOBAL = 1;
		char * sms2 = "\nExiting foreground-only mode\n";
		write(STDOUT_FILENO, sms2, 30);
	}
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
nonBuiltParse_Zombie()
*/
void nonBuiltParse_Zombie(char** arr, char** argv, int arg_count){
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
		//https://stackoverflow.com/questions/14846768/in-c-how-do-i-redirect-stdout-fileno-to-dev-null-using-dup2-and-then-redirect//
		//opening/closing to redirect dev/null
		int dev_null = open("/dev/null",O_WRONLY);
		result = dup2(dev_null, 0);
		if(result == -1) {
			perror("Source in failed");
			exit(2);
		}
		result = dup2(dev_null,1);
		if(result == -1) {
			perror("Out target failed");
			exit(2);
		}
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
	int ex_sig, ex_stat;
	int childExitStatus= -10;
	spawnPID = fork();
	child_HX.PID=spawnPID;
		if(spawnPID == -1){
			perror("You goof'd\n");
			exit(1);
		}
		else if (spawnPID == 0){
			char* argv[512];
			memset(&argv, 0 , sizeof(argv));
			//If theres an ampersand + TSPT triggered we run it with 1 less argu aka the &
			if(amp_check(arr, arg_count) && !(TSPT_GLOBAL))
				nonBuiltParse(arr,argv,arg_count-1);
			else
				nonBuiltParse(arr,argv,arg_count);
			if(execvp(*argv,argv) < 0){
				perror("Bad Command");
				fflush(stderr);
				exit(1);
			}
		}
		int x = waitpid(spawnPID, &(child_HX.childExitStat), 0);
}

/*
zombie_land
Alteration from normal exec function ( not_my_problem) we have to readjust and run our child processes in the background now
Input: pointer to char of our string arguments , int argument count , and int array to store the child processes in. 
Output: void function no return.
*/
void zombie_land( char** arr, int* hist, int arg_count){
	pid_t spawnPID = -5;
	int childExit = -5;
	spawnPID = fork();
	int j;
	for(j = 0; j < 10; j++){
		if(hist[j] == -1){
			hist[j] = spawnPID;
			break;
		}
	}
	
	if(spawnPID == -1){
		perror("You goof'd\n");
		exit(1);
	}
	else if(spawnPID == 0){
		char* argv[512];
		memset(&argv, 0, sizeof(argv));
		printf("background pid is %i\n: ", getpid());
		fflush(stdout);
		nonBuiltParse_Zombie(arr,argv,arg_count-1);
		if(execvp(*argv,argv) < 0){
			perror("Bad Command");
			fflush(stderr);
			exit(1);
		}
	}
	//waitpid(spawnPID, &childExit, 0);
}

/*
double_tap()
Scans our background PID to see if anything is done and should be cleaned up.
Input: Takes an array of ints containing our spawn'd PIDS
Outputs: Void function so nothing, excepts print to screen processes to which are done
*/
double_tap(int* PID){
	int j;
	int wp,ex_stat, ex_sig;
	int childExit = -5;
	for(j = 0; j < 10; j++){
		if(PID[j] != -1){
			if(waitpid(PID[j], &childExit, WNOHANG)){
				if( WIFEXITED(childExit)){
					printf("background pid %i is done: ", PID[j]);
					ex_stat = WEXITSTATUS(childExit);
					printf("exit status was %i\n",ex_stat);
					fflush(stdout);
				}
				if(WIFSIGNALED(childExit) != 0){
					printf("background pid %i is done: ",PID[j]);
					ex_sig = WTERMSIG(childExit);
					printf("terminated with signal %i\n",ex_sig);
					fflush(stdout);
				}
				PID[j] = -1;
			}
			
		}
	}
}

/*
amp_check
*/
int amp_check(char** arr, int arg_count){
	if(strcmp(arr[arg_count-1],"&") == 0){
		return 1;
	}
	return 0;
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
void arg_Process(char** arr, int arg_count, int *bgPID){
	char * command = arr[0];
	int x,ex_stat,ex_sig;
	if(strcmp( command, "exit") == 0){
		for (x = 0; x < 10; x++){
			if(bgPID[x] != -1){
				kill(bgPID[x], 15); // SIGTERM : https://linux.die.net/man/3/kill
				bgPID[x] = -1;
			}
		}
		exit(0);
	}
	else if(strcmp(command, "cd") == 0){
		cd_handler(arr);
	}
	else if (strcmp( command, "status") == 0){
		 if(WIFEXITED(child_HX.childExitStat)){
			 ex_stat = WEXITSTATUS(child_HX.childExitStat);
			printf("exit value %i\n",ex_stat);
			fflush(stdout);
		}
		else{
			if(WIFSIGNALED(child_HX.childExitStat) != 0){
				ex_sig = WTERMSIG(child_HX.childExitStat);
				printf("terminated with signal %i\n", ex_sig);
				fflush(stdout);
			}
		}
	}
	else{
		if(amp_check(arr,arg_count) && TSPT_GLOBAL) // TSPT when signaled will turn 1 and 0 
			zombie_land(arr, bgPID, arg_count);
		else
			not_my_problem(arr, arg_count);
	}
}


/*
money_maker : NOT FUNCTIONAL post expansion instead of PRE use maker II instead
-Checks for any $$ and gets the shell PID
Input: array of our parsed arguments and # of arguments
Output: no returns, just print to screen of PID 
https://fresh2refresh.com/c-programming/c-type-casting/c-itoa-function/
https://stackoverflow.com/questions/12970439/gcc-error-undefined-reference-to-itoa
*/
void money_maker(char** arr, int x){
	//convert pid into string//
	//search for $$ and replace with pID string if found//
	int y;
	int pID = getpid();
	char buff[10] = {'\0'};
	sprintf(buff, "%i", pID);
	for(y = 0; y < x; y++){
		if(strcmp( arr[y], "$$") ==0){
			strcpy(arr[y],buff);
		}
	}
}
/*
money_maker_II
Takes our command line and expands all $$ in to our PID before strtok
input: our array of char for command input
output: void return, does a string copy back into the original command array tho.
*/
void money_maker_II(char* cmd_in){
	char buff[2048], PID[10];
	memset(buff, '\0', sizeof(buff));
	memset(PID, '\0', sizeof(PID));
	int pID = getpid();
	sprintf(PID, "%i", pID);
	int x = 0; // cmd in ind
	int y = 0; // buffer index
	int z = 0; // pid buffer
	while(cmd_in[x] != '\0'){
		if(cmd_in[x] == '$' && cmd_in[x+1] == '$'){
			while(PID[z] != '\0'){
				buff[y] = PID[z];
				y++;
				z++;
			}
			z=0;
			x++;
		}
		else
			buff[y] = cmd_in[x];
		x++;
		y++;
	}
	strcpy(cmd_in,buff);
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
	int bgPID[10];
	memset(bgPID, -1, sizeof(bgPID));
	
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
			money_maker_II(cmd_length);
			arg_Parser(cmd_length, arguments, &arg_count); //parse command into args
			//money_maker(arguments, arg_count);
/* 			if(amp_check(arguments,arg_count)){
				zombie_land(arguments, bgPID, arg_count);
			}
			else */
				arg_Process(arguments, arg_count, bgPID);
		}
		double_tap(bgPID);			//cleans up possible background PID aka double tab all the potential zombies
		arg_count = 0;
	}
}

int main(){
	TSPT_GLOBAL = 1;
	child_HX.childExitStat = -5;
	
	struct sigaction SIGINT_action = {0};
	struct sigaction SIGTSTP_action = {0};
	
	SIGINT_action.sa_handler=catchSIGINT;
	sigfillset(&SIGINT_action.sa_mask);
	SIGINT_action.sa_flags=SA_RESTART;
	
	SIGTSTP_action.sa_handler = catchSIGTSTP;
	sigfillset(&SIGTSTP_action.sa_mask);
	SIGTSTP_action.sa_flags=0;
	
	//Registers sigint to siginit action struct //
	sigaction(SIGINT, &SIGINT_action, NULL);
	sigaction(SIGTSTP,&SIGTSTP_action,NULL);
	
	loop_sh();
	return 0;
}