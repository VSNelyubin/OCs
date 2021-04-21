#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#define PIPE_READ 0
#define PIPE_WRITE 1

int main(){
	int num=255;
	char*inpt2=malloc(sizeof(char)*num);
	char*inpt;
	fgets (inpt2, num, stdin);
	for(int i=0;i<num;i++){
		if(inpt2[i]=='\n'){
			num=i;
			inpt=malloc(sizeof(char)*num);
			for(int ee=0;ee<num;ee++){
				inpt[ee]=inpt2[ee];
			}
			free(inpt2);
			break;
		}
	}

	if( access( inpt, F_OK ) ) {
		printf("Error: file not found\n");
		return -1;
	}

	pid_t pid;
	int status;

	int aPipe[2];
  	if (pipe(aPipe) < 0) {
    		close(aPipe[PIPE_READ]);
    		close(aPipe[PIPE_WRITE]);
    		perror("allocating pipe for child output redirect");
    		return -1;
  	}

	pid=fork();
	if(pid==0){
		close(aPipe[PIPE_READ]);
    		if (dup2(aPipe[PIPE_WRITE], STDOUT_FILENO) == -1) {
    			exit(errno);
    		}

		char*args[]={"./b.out",inpt,NULL};
		execvp(args[0],args);
		_exit (EXIT_FAILURE);
	}
	else if(pid<0){
		printf("something went wrong with fork()\n");
	}
	else{
		close(aPipe[PIPE_WRITE]);
		num=1;
		inpt=realloc(inpt,sizeof(char)*num);
		int n;
		while((n=read(aPipe[PIPE_READ],inpt,num))!=0){
			if(inpt[0]=='-'){break;}
			printf("%c",inpt[0]);
		}
		close(aPipe[PIPE_READ]);
	}
	return 0;
}
