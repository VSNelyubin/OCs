#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int w(int sb){
//printf("entered w\n");
	for(int i=2;i*i<sb;i++){
		if((sb%i)==0){
			//printf("%d\n",sb);
			return 0;
		}
	}
//	printf("%d\n",-1);
	return 1;
}

int main( int argc, char *argv[] ){
/*	printf("\n");
	for(int i=0;i<argc;i++){
		for(int ee=0;argv[i][ee-(ee!=0)]!='\0';ee++){
			if(argv[i][ee]=='\0'){printf("\\0");}
			else{printf("%c",argv[i][ee]);}
		}
		printf("\n");
	        //printf("\n%s\n",argv[i]);
        }*/
	if( argc != 3 ) {
		printf("bad command line arguments :(\n");
		exit(-1);
	}
	int fd = open(argv[1], O_RDWR);
        if (fd == -1){printf("error opening file\n");}
	char ch;

        size_t sz=0;
        for(int ee=0;argv[2][ee]!='\0';ee++){
                sz=sz*10+argv[2][ee]-'0';
        }
	char*file=mmap(NULL, sz, PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);//mmap(NULL, sz, PROT_RDWR,MAP_SHARED, fd,0);

/*	char*file;
	int subj;
	unsigned long long fltp=0;
	for(int ee=0;argv[1][ee]!='\0';ee++){
//		printf("%c",argv[1][ee]);
		subj=0;
        	if((argv[1][ee]>='0')&&(argv[1][ee]<='9')){
			subj=(int)argv[1][ee];
			subj-='0';
//			printf("-N=");
		}
                if((argv[1][ee]>='a')&&(argv[1][ee]<='z')){
			subj=10+(int)argv[1][ee];
                        subj-='a';
//			printf("-C=");
		}
//		printf("%d\n",subj);
		fltp=fltp<<5;fltp+=subj;
        }
	file=(char*)fltp;
//	printf("\n%llu\n",file);
*/
//	printf("sz=%d\n",sz);

//	char*acces=mmap((void*)file, sz, PROT_READ,MAP_SHARED|MAP_ANONYMOUS, -1, 0);

	int wrtnm=0;

	for(int i=0;i<sz;i++){
		if(file[i]=='\n'){
			if(w(wrtnm)){
				file[i]='N';
			}
			else{
				file[i]='Y';
			}
			i++;
			wrtnm=0;
		}
		wrtnm=wrtnm*10+file[i]-'0';
//                printf("%c",file[i]);
//		if((file[i]>='0')&&(file[i]<'9')){file[i]++;}
        }//printf("\n");
	return 0;
}
