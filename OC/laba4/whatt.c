#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>


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
//				printf("%d\n",((inpt2[ee]=='\0')||(inpt2[ee]=='\n')));
			}
			free(inpt2);
			break;
		}
	}

	int fd = open(inpt, O_RDWR);
        if (fd == -1){printf("error opening file\n");}
	char ch;

        size_t sz;
	sz = lseek(fd, 0, SEEK_END);

	void*mapped=mmap(NULL, sz, PROT_READ|PROT_WRITE,MAP_SHARED, fd,0);
	close(fd);

	if(mapped==NULL){printf("bad mmap\n");return -1;}
//	else{printf("good mmap @ %llu\n",(unsigned long long)mapped);}

	char*flhr=(char*)mapped;

	char*backup=malloc(sizeof(char)*sz);
	if(backup==NULL){return -1;}
	for(int i=0;i<sz;i++){
		backup[i]=flhr[i];
	}

/*	for(int i=0;i<sz;i++){
		printf("%c ",flhr[i]);
	}printf("\n");*/

	pid_t pid;

/*	unsigned long long adres=(unsigned long long)mapped;
	for(int i=0;adres>>i;i++){printf("%d",(adres>>i)&1);
		if(i%5==4){printf("\t");}
	}
	printf("\n");
	for(int i=0;adres>>i;i+=5){printf("%d\t",(adres>>i)&31);}
	printf("\n");

	printf("%llu %llu\n", mapped,adres);
*/

	int ci,cim=0;

	char*szarg;
	for(ci=1;ci<sz;ci*=10){
		cim++;
	}
	szarg=malloc(sizeof(char)*(1+cim));
	szarg[cim]='\0';
	cim=0;
	for(ci=ci/10;ci;ci/=10){
		szarg[cim]='0'+(sz/ci)%10;
                cim++;
        }

/*	for(ci=0;adres>>(5*ci);ci++);
	inpt=realloc(inpt,sizeof(char)*ci);
	cim=ci;
	ci--;
	for(ci;ci+1;ci--){
		fd=(adres>>(5*ci))&31;
		if(fd<10){fd+='0';}
		else{fd+='a'-10;}
		inpt[cim-ci-1]=fd;
	}
	inpt[cim]='\0';

	printf("not split yet %llu\n",flhr);
        for(int i=0;i<sz;i++){
        	printf("%c ",flhr[i]);
        }printf("\n");*/

//	printf("inpt is %s\n",inpt);

	pid=fork();
	if(pid==0){
//		printf("child, but not B yet %llu\n",flhr);
		char*args[]={"./b.out",inpt,szarg,NULL};
		execvp(args[0],args);
		_exit (EXIT_FAILURE);
	}
	else if(pid<0){
		printf("something went wrong with fork()\n");
	}
	else{
		ci=0,cim=0;
		while(ci<sz){
			while((flhr[ci]!='\n')&&(flhr[ci]!='Y')&&(flhr[ci]!='N')){
				cim=cim*10+flhr[ci]-'0';
				ci++;
			}
			while(flhr[ci]=='\n');
			if(flhr[ci]=='Y'){
				printf("%d\n",cim);
				cim=0;
				ci++;
			}
			if(flhr[ci]=='N'){
				break;
			}
		}
		printf("prime number\n");
		kill(pid, SIGKILL);
		for(int i=0;i<sz;i++){
                	flhr[i]=backup[i];
        	}
		free(backup);
		munmap(mapped,sz);
	}
	return 0;
}
