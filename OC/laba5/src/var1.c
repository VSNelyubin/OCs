int primeCount(int A,int B){
	int rez=0,i,j;
	for(i=A;i<=B;i++){
		for(j=2;j<i;j++){
			if((i%j)==0){
				j=0;
				break;
			}
		}
		if(j){
			rez++;
		}
	}
	return rez;
}

float E(int x){
	double b,ex=1;
	b=1/(double)x +1;
	for(int i=0;i<x;i++){
		ex=ex*b;
	}
	return (float)ex;
}
