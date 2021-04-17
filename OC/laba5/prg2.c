#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

//int primeCount(int A,int B);
//float E(int x);

int main( int argc, char *argv[] ){

	int (*primeCount)(int, int);
	float (*E)(int);

	if( argc != 2 ) {
		printf("bad command line arguments :(\n");
		exit(-1);
	}
	char mode=argv[1][0];
	if((mode!='1')&&(mode!='2')){
		printf("bad comment. arguments should be either 1 or 2\n");
		exit(-1);
	}
	char*lib1="libvar1.so";
	char*lib2="libvar2.so";

	void*copen;

	if(mode=='1'){
		copen=dlopen(lib1, RTLD_LAZY);
		mode='2';
	}
	else{
		copen=dlopen(lib2,RTLD_LAZY);
		mode='1';
	}
	if(NULL==copen){
		printf("cannot open dynamic library\n");
		exit(-1);
	}

	*(void **) (&E) = dlsym(copen, "E");
        *(void **) (&primeCount) = dlsym(copen, "primeCount");

	int num=255,ag1,ag2,i;
	char*inpt=malloc(sizeof(char)*num);
	while(NULL!=fgets (inpt, num, stdin)){
		if(inpt[0]=='\n'){
			break;
		}
		if(inpt[0]=='0'){
			printf("change library here\n");
			if(mode=='1'){
				dlclose(copen);
        		        copen=dlopen(lib1, RTLD_LAZY);
				mode='2';
		        }
		        else{
				dlclose(copen);
		                copen=dlopen(lib2,RTLD_LAZY);
				mode='1';
		        }
		        if(copen==0){
		                printf("cannot open dynamic library\n");
	        	        exit(-1);
		        }
			*(void **) (&E) = dlsym(copen, "E");
	                *(void **) (&primeCount) = dlsym(copen, "primeCount");
			continue;
		}
		if(inpt[0]=='1'){
			ag1=0;
			ag2=0;
			for(i=2;((inpt[i]>='0')&&(inpt[i]<='9'));i++){
				ag1=ag1*10+inpt[i]-'0';
			}
			i++;
			for(i;((inpt[i]>='0')&&(inpt[i]<='9'));i++){
                                ag2=ag2*10+inpt[i]-'0';
                        }
			printf("there are %d prime numbers between %d and %d\n",primeCount(ag1,ag2),ag1,ag2);
			continue;
		}

		if(inpt[0]=='2'){
                        ag1=0;
                        ag2=0;
                        for(i=2;((inpt[i]>='0')&&(inpt[i]<='9'));i++){
                                ag1=ag1*10+inpt[i]-'0';
                        }
                        printf("e is %f with %d iterations\n",E(ag1),ag1);
                        continue;
                }

		/*for(int i=0;i<num;i++){
			printf("%c",inpt[i]);
		}
		printf("\n");*/
	}
	return 0;
}
