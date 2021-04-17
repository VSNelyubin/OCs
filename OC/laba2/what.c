#include <stdio.h>
#include <stddef.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define SHELL "/bin/sh"

int my_sys(const char*command){
	int status;
	pid_t pid;

	pid=fork();
	if(pid==0){
		execl (SHELL, SHELL, "-c",command,NULL);
		_exit (EXIT_FAILURE);
	}
	else if(pid<0){
		status=-1;
	}
	else{
		if(waitpid(pid,&status,0)!=pid){
			status=-1;
		}
	}
	return status;
}


int main(){
	char*inpt,fname;
	fname="input.txt"
	int fId;
	if((fId=open(fname,O_RDONLY))<0){
		printf("no such file\n");
		perror(fname);
		exit(-1);
	}
	my_sys("./b.out");
	return 0;
}
