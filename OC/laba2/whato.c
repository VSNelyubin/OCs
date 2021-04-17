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

//printf("\n\n\n ./b.out began \n\n\n");

	for(int i=0;i<argc;i++){
/*		for(int ee=0;argv[i][ee-(ee!=0)]!='\n';ee++){
			if(argv[i][ee]=='\n'){printf("\\n");}
			else{printf("%c\n",argv[i][ee]);}
		}
*/	        //printf("\n%s\n",argv[i]);
        }
	if( argc != 2 ) {
		printf("bad command line arguments :(\n");
		exit(-1);
	}
//	FILE* fp = fopen(argv[1], "r");
//printf("test a\n");
	freopen(argv[1], "r",stdin);
//printf("test b\n");
	int subj;
//	fscanf (fp, "%d", &subj);
//	while (!feof (fp)){
//printf("test c\n");
	while (scanf("%d", &subj) == 1){
//printf("test d\n");
		if(w(subj)){
			break;
		}
		//fscanf (fp, "%d", &subj);
	}
//  	fclose (fp);

//	printf("\n\n\n ./b.out ended \n\n\n");

	return 0;
}
