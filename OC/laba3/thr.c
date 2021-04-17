#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct cci{
	char*r;
	char*w;
	int o;
}cci;

void* isSubstr(void*vdi){//char*where,char*what,int offset){
	cci*inpt=(cci*)vdi;
	while(inpt->o==-1){}
//printf("%d\n",vdi);
//printf("%d\n",((cci*)inpt)->o);
//	printf("%s\n",inpt->r);for(int i=0;i<inpt->o;i++){printf(" ");}printf("%s\n",inpt->w);
	for(int i=0;inpt->w[i]!='\0';i++){
		if(inpt->r[inpt->o+i]!=inpt->w[i]){
			inpt->o=-1;
			break;
		}
	}
	if(inpt->o!=-1){
		printf("sample found at %d\n",inpt->o);
	}
}

int main(int argc, char *argv[]){

	int limt,strl,smpl;
	if(argc < 4) {
		printf("bad command line arguments :(\n");
		return -1;
	}
	for(smpl=0;argv[3][smpl]!='\0';smpl++);
	for(strl=0;argv[2][strl]!='\0';strl++);
        if(smpl>strl){printf("sample is bigger than the line. very bad\n");return -1;}
	limt=0;
	for(int i=0;argv[1][i]!='\0';i++){
		limt=limt*10+argv[1][i]-'0';
	}

	printf("max threads = %d\nline = %s\nsample = %s\n",limt,argv[2],argv[3]);

	pthread_t*thrds=malloc(sizeof(pthread_t)*limt);
	int rertn,ei,ii;

	cci**fori=malloc(sizeof(cci*)*limt);
	for(ei=0;ei<limt;ei++){
		fori[ei]=malloc(sizeof(cci));
	}

        for(ei=0;ei<=strl-smpl;ei++){
		ii=ei%limt;
                if(ei>=limt){
//			while(argc>4){}
                        if(pthread_join( thrds[ii], NULL)!=0){printf("join error at i=%d\n",ii);}
                }
                fori[ii]->r=argv[2];
                fori[ii]->w=argv[3];
                fori[ii]->o=ei;
		if(argc>4){fori[ii]->o=-1;}
//		printf("\n--\n%s\n",fori->r);for(int i=0;i<fori->o;i++){printf(" ");}printf("%s\n--\n",fori->w);
                if(pthread_create( &thrds[ii], NULL, isSubstr, (void*)fori[ii])!=0){printf("call error at i=%d\n",ii);}
        }
	ii++;
	if(ei!=ei%limt){
		ii=limt;
	}
	for(int j=0;j<ii;j++){
        	if(pthread_join( thrds[j], NULL)!=0){printf("join error at i=%d\n",j);}
        }
	for(ei=0;ei<limt;ei++){
                free(fori[ei]);
        }
	free(fori);
	free(thrds);
	return 0;
}
