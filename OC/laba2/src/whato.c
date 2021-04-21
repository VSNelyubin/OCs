#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <unistd.h>

int w(int sb){
//printf("entered w\n");
	for(int i=2;i*i<sb;i++){
		if((sb%i)==0){
			printf("%d\n",sb);
			return 0;
		}
	}
	printf("%d\n",-1);
	return 1;
}

int main( int argc, char *argv[] ){
	if( argc != 2 ) {
		printf("bad command line arguments :(\n");
		exit(-1);
	}
	freopen(argv[1], "r",stdin);
	int subj;
	while (scanf("%d", &subj) == 1){
		if(w(subj)){
			break;
		}
	}
	return 0;
}
