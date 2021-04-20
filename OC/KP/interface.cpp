#include <iostream>
#include <chrono>
#include <thread>
#include <string>
#include <map>
#include <vector>
#include <cstring>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif
#include <zmq.hpp>

const size_t TCPE=32;


int main( int argc, char *argv[] ){
	char*NodeExec=new char(10);
	for(int i=0;i<10;i++){
		NodeExec[i]="./c.out\0"[i];
	}
	int num=255,i,j;
	zmq::context_t context (1);
	zmq::socket_t toC (context,ZMQ_REQ);
	zmq::socket_t toB (context,ZMQ_PUSH);
	toB.connect(argv[1]);
	toC.bind("tcp://*:*");//*/
	char*adresC=new char[TCPE];
	size_t tmpsizeN=TCPE*sizeof(char);
	if(zmq_getsockopt(toC,ZMQ_LAST_ENDPOINT,adresC,&tmpsizeN)){
		toB.close();
		toC.close();
		delete[] adresC;
		printf("socket T ID failure1\n");
		return -1;
	}

	pid_t pidN;
	pidN=fork();
	if(pidN==0){
		char*args[]={NodeExec,adresC,argv[1],NULL};
		execvp(args[0],args);
		_exit (EXIT_FAILURE);
	}
	else if(pidN<0){
		printf("something went wrong with fork()\n");
		toC.close();
		delete[] adresC;
		return -1;
	}
	char*inpt=new char[num];
	while(NULL!=fgets (inpt, num, stdin)){
		zmq::message_t request(num);
		memcpy(request.data (), inpt, num);
//		((char*)request.data())[request.size()]='\0';
//		toC.send(request,zmq::send_flags::none);

		int timp;
		for(timp=0;((char*)request.data())[timp]!='\0';timp++);
		std::string temp="A sent "+std::to_string(timp)+" symbols\0";//reply.size())+"\0";
		zmq::message_t request2 (temp.size()+1);
		memcpy (request2.data (), temp.c_str(), temp.size()+1);
		toB.send(request2,zmq::send_flags::none);

		((char*)request.data())[request.size()]='\0';
		toC.send(request,zmq::send_flags::none);

		zmq::message_t reply;
		toC.recv(reply,zmq::recv_flags::none);
		((char*)reply.data())[reply.size()]='\0';
		printf("A: %s\n",(char*)reply.data());

/*		zmq::message_t request2 (4);
		memcpy (request2.data (), "Aes\0", 4);
		toB.send(request2,zmq::send_flags::none);
*/	}

	zmq::message_t fnmsg(4);
	memcpy (fnmsg.data (), "\nN\0", 3);
	((char*)fnmsg.data())[fnmsg.size()]='\0';
	toB.send(fnmsg,zmq::send_flags::none);

	zmq::message_t fnmsg2(4);
	memcpy (fnmsg2.data (), "\nN\0", 3);
	((char*)fnmsg2.data())[fnmsg2.size()]='\0';
	toC.send(fnmsg2,zmq::send_flags::none);
	printf("A ended\n");
	toB.close();
	toC.close();
	context.close();
	return 0;
}

