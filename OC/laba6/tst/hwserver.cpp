//
//  Hello World server in C++
//  Binds REP socket to tcp://*:5555
//  Expects "Hello" from client, replies with "World"
//
#include <zmq.hpp>
#include <string>
#include <iostream>
#ifndef _WIN32
#include <unistd.h>
#else
#include <windows.h>

#define sleep(n)	Sleep(n)
#endif

const size_t TCPE=32;

int main () {
//      Prepare our context and socket
//    	zmq::context_t context (1);
//    	zmq::socket_t socket (context, ZMQ_REP);
//    	socket.bind ("tcp://*:5555");

	zmq::context_t context (1);
        zmq::socket_t socket (context, ZMQ_REP);
        socket.bind ("tcp://*:*");
        char*tmpadres=new char[TCPE];
	tmpadres[0]='e';
	size_t tmpsize=TCPE*sizeof(char);
//printf("before\n");
	int fq;
	fq=zmq_getsockopt(socket,ZMQ_LAST_ENDPOINT,tmpadres,&tmpsize);
//        printf("%d %s %lu\n",fq,tmpadres,tmpsize);
//        return 0;


    	pid_t pid;

	pid=fork();
	if(pid==0){
		char*NodeExec=new char(10);
		for(int i=0;i<10;i++){
			NodeExec[i]="./clin.out"[i];
		}
		char*args[]={NodeExec,tmpadres,NULL};
		execvp(args[0],args);
		_exit (EXIT_FAILURE);
	}
	else if(pid<0){
		printf("something went wrong with fork()\n");
	}
	else{

		while (true) {
		        zmq::message_t request;

        //  Wait for next request from client
		        socket.recv (&request);
		        std::cout << "Received Hello" << std::endl;

        //  Do some 'work'
		    	sleep(1);

        //  Send reply back to client
		        zmq::message_t reply (5);
		        memcpy (reply.data (), "World", 5);
		        socket.send (reply);
	    	}
	}
    return 0;
}
