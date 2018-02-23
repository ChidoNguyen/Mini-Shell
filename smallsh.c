/*
Chido Nguyen
931506965
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>


/*
loop_sh
-our shell emulator that loops until exit is called
*/
void loop_sh(){
	char cmd_length[2048];
	char* arguments[512];
	while(1){
		printf(": ");
		fflush(stdout);
		fgets(cmd_length , 2048 , stdin);
	}
}

int main(){
	loop_sh();
	return 0;
}