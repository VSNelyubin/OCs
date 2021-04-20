#include <iostream>
#include <thread>
#include <time.h>
#include <string>
#include <vector>
#include <map>
#include <cstring>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>
#define sleep(n)	Sleep(n)
#endif
#include <zmq.hpp>

const size_t TCPE=32;

int main(){// int argc, char *argv[] ){
	char*NodeExec=new char(10);
	for(int i=0;i<10;i++){
		NodeExec[i]="./a.out\0"[i];
	}
	zmq::context_t context (1);
	zmq::socket_t sci (context, ZMQ_PULL);
	sci.bind("tcp://*:*");//*/
	char*adresC=new char[TCPE];

	size_t tmpsizeN=TCPE*sizeof(char);
	if(zmq_getsockopt(sci,ZMQ_LAST_ENDPOINT,adresC,&tmpsizeN)){
		sci.close();
		delete[] adresC;
		printf("socket T ID failure1\n");
		return -1;
	}

	pid_t pidN;
	pidN=fork();
	if(pidN==0){
		char*args[]={NodeExec,adresC,NULL};
		execvp(args[0],args);
		_exit (EXIT_FAILURE);
	}
	else if(pidN<0){
		printf("something went wrong with fork()\n");
		sci.close();
		delete[] adresC;
		return -1;
	}

	zmq::message_t reply;
	while(1){
//		printf("b awaits answer\n");
		sci.recv(reply,zmq::recv_flags::none);
//		((char*)reply.data())[reply.size()]='\0';
		if(((char*)reply.data())[0]=='\n'){
			break;
		}
		printf("B: %s\n",(char*)reply.data());
	}
	printf("B ended\n");
	sci.close();
	context.close();
	return 0;
}
