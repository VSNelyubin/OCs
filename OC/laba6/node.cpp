#include <zmq.hpp>
#include <string>
#include <iostream>

int main( int argc, char *argv[] ){
/*	printf("arguments:\n");
	for(int i=0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
	printf("arguments over\n");*/

	zmq::context_t context (1);
	zmq::socket_t psocket (context, ZMQ_REP);

//	std::cout << "Connecting to parent..." << std::endl;
	psocket.connect (argv[1]);

	zmq::message_t reply;
        psocket.recv(reply,zmq::recv_flags::none);
	printf("parent: %s\n",(char*)reply.data());

	if( argc != 3 ) {
		zmq::message_t request (8);
	        memcpy (request.data (), "BadInit\0", 8);
        	psocket.send(request,zmq::send_flags::none);
		psocket.close();
        	context.close();
		exit(-1);
        //        printf("bad command line arguments :(\n");
        }

        int CurId=0;
        for(int i=(argv[2][0]=='-');argv[2][i]!='\0';i++){
                CurId=CurId*10+argv[2][i]-'0';
        }
        if(argv[2][0]=='-'){
                CurId*=-1;
        }


	zmq::message_t request (6);
        memcpy (request.data (), "Ready\0", 6);
        psocket.send(request,zmq::send_flags::none);

//	std::cout<<"Can safely start working\n";

	while(1){
		psocket.recv (reply);
        	printf("child received \"%s\"\n",(char*)reply.data());

		zmq::message_t request (4);
        	memcpy (request.data (), "g2g\0", 4);
	        psocket.send(request,zmq::send_flags::none);

		if(((char*)reply.data())[0]==6){
			break;
		}
	}
	psocket.close();
	context.close();
	return 0;
}
