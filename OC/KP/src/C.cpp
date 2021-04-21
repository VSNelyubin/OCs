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



int main( int argc, char *argv[] ){
/*	printf("\nc arguments:\n");
	for(int i=0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
	printf("arguments over\n\n");
*/
	zmq::context_t context (1);
	zmq::socket_t toA (context, ZMQ_REP);
	zmq::socket_t toB (context, ZMQ_PUSH);
	toA.connect (argv[1]);
	toB.connect (argv[2]);
	std::string temp;
	zmq::message_t reply;
	int timp;
	while(1){
		toA.recv(reply,zmq::recv_flags::none);
		((char*)reply.data())[reply.size()]='\0';
		/*if(((char*)reply.data())[0]=='N'){
			break;
		}*/
		printf("C: %s\n",(char*)reply.data());
		for(timp=0;((char*)reply.data())[timp]!='\0';timp++);
		temp="C reeived "+std::to_string(timp)+" symbols\0";//reply.size())+"\0";


		zmq::message_t request (temp.size()+1);
		memcpy (request.data (), temp.c_str(), temp.size()+1);
//printf("c send to b\n");
		toB.send(request,zmq::send_flags::none);
//printf("c sent to b\n");

		zmq::message_t request2 (4);
		memcpy (request2.data (), "Yes\0", 4);
		toA.send(request2,zmq::send_flags::none);


		if(((char*)reply.data())[0]=='\n'){
			break;
		}
	}
	printf("C ended\n");
	toA.close();
	toB.close();
	context.close();
	return 0;
}
