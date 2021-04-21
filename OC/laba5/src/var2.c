#include <math.h>
#include <stdio.h>
#include <stdlib.h>

void fil(int*ar,int pos){
        int ee=pos/(8*sizeof(int));
        int id=pos-ee*8*sizeof(int);
        ar[ee]=ar[ee]|(1<<id);
}

int chek(int*ar,int pos){
        int ee=pos/(8*sizeof(int));
        int id=pos-ee*8*sizeof(int);
        return !((ar[ee])&(1<<id));
}

int primeCount(int A,int B){
        int rez=0,i,j;
        for(i=0;sizeof(int)*8*i<B;i++);
        int*tbl=malloc(sizeof(int)*i);
        for(j=0;j<i;j++){
                tbl[j]=0;
        };
        for(j=2;j<=B;j++){
                if(chek(tbl,j)){
			if(j>1){
	                        for(i=j*2;i<=B;i+=j){
        	                        fil(tbl,i);
                	        }
			}
                        if(j>=A){
                                rez++;
                        }
		}
	}
        free(tbl);
        return rez;
}

float E(int x){
	double l=1,rez=1;
	for(int n=1;n<x;n++){
		l/=n;);
		rez+=l;
	}
	return (float)rez;
}
