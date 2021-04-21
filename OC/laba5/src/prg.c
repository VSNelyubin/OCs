#include <stdio.h>
#include <stdlib.h>

int primeCount(int A,int B);
float E(int x);

int main(){
	int num=255,ag1,ag2,i;
	char*inpt=malloc(sizeof(char)*num);
	while(NULL!=fgets (inpt, num, stdin)){
		if(inpt[0]=='\n'){
			break;
		}
		if(inpt[0]=='0'){
			printf("change library here\n");
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
	}
	return 0;
}
