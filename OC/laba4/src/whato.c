#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>

int w(int sb){
	for(int i=2;i*i<sb;i++){
		if((sb%i)==0){
			return 0;
		}
	}
	return 1;
}

int main( int argc, char *argv[] ){
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
	char*file=mmap(NULL, sz, PROT_READ|PROT_WRITE,MAP_SHARED, fd, 0);
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
        }
	return 0;
}
