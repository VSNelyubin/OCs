#include <iostream>


//#include <thread>
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

const int CMNM=6;
const size_t TCPE=32;

void killChild(std::vector<pid_t> &pidsN , std::vector<int> &chidsN , std::vector<zmq::socket_t> &socketsN,int idx){
	socketsN[idx].close();
	pidsN[idx]=-1;
	chidsN[idx]=-1;
	/*for(int i=idx;i<pidsN.size()-1;i++){
		pidsN[i]=pidsN[i+1];
		chidsN[i]=chidsN[i+1];
		socketsN[i]=socketsN[i+1];
	}*/
	if(idx==pidsN.size()){
		pidsN.resize(pidsN.size()-1);
		chidsN.resize(chidsN.size()-1);
		socketsN.resize(socketsN.size()-1);
	}
}

bool sockReset(zmq::socket_t &socketN,zmq::context_t&contextN){
	char*tmpadresN=new char[TCPE];
	size_t tmpsizeN=TCPE*sizeof(char);
	int fqN;
	int pulse;
	fqN=zmq_getsockopt(socketN,ZMQ_LAST_ENDPOINT,tmpadresN,&tmpsizeN);
	fqN=zmq_getsockopt(socketN,ZMQ_RCVTIMEO,&pulse,&tmpsizeN);
	socketN.close();
	socketN=zmq::socket_t(contextN,ZMQ_REQ);
	socketN.bind(tmpadresN);
	fqN=zmq_setsockopt(socketN,ZMQ_RCVTIMEO,&pulse,tmpsizeN);
	return true;
}

bool newChildNode(std::vector<pid_t> &pidsN , std::vector<int> &chidsN , std::vector<zmq::socket_t> &socketsN , zmq::context_t&contextN , char*NodeExecN,int chidN, int*pulse){
	int idx;//=socketsN.size();
	for(idx=0;idx<socketsN.capacity();idx++){
		if((pidsN[idx]==-1)&&(chidsN[idx]==-1)){
			break;
		}
	}
	if(socketsN.capacity()==idx){
		socketsN.resize(idx+1);
	}
	socketsN[idx]=zmq::socket_t(contextN,ZMQ_REQ);
	socketsN[idx].bind ("tcp://*:*");//*/
	char*tmpadresN=new char[TCPE];
	size_t tmpsizeN=TCPE*sizeof(char);
	int fqN;
//	fqN=socketsN[idx]->get(zmq::sockopt::last_endpoint);//,tmpadresN,&tmpsizeN);
	fqN=zmq_getsockopt(socketsN[idx],ZMQ_LAST_ENDPOINT,tmpadresN,&tmpsizeN);
//printf("fcp=%s\n",tmpadresN);
	if(fqN){
		socketsN[idx].close();
//		delete socketsN[idx];
		socketsN.resize(idx);
		delete[] tmpadresN;
		printf("socket ID failure %d\n",fqN);
		return false;
	}
//	fqN=socketsN[idx]->setsockopt(ZMQ_RCVTIMEO,pulse,sizeof(int));
	fqN=zmq_setsockopt(socketsN[idx],ZMQ_RCVTIMEO,pulse,sizeof(int));
	if(fqN){
		socketsN[idx].close();
//		delete socketsN[idx];
		socketsN.resize(idx);
		delete[] tmpadresN;
		printf("socket Time failure %d\n",fqN);
		return false;
	}

	pid_t pidN;
	pidN=fork();
	if(pidN==0){
		std::string vtmp=std::to_string(chidN)+"\0\n";
		char*chidAr=new char[16];
		for(int chi=0;vtmp[chi]!='\0';chi++){
			chidAr[chi]=vtmp[chi];
			chidAr[chi+1]='\0';
		}
		char*args[]={NodeExecN,tmpadresN,chidAr,NULL};
//		for(int ee=0;ee<3;ee++){
//			printf("%s\n",args[ee]);
//		}printf("\n");
		execvp(args[0],args);
		_exit (EXIT_FAILURE);
	}
	else if(pidN<0){
		printf("something went wrong with fork()\n");
		socketsN[idx].close();
		socketsN.resize(idx);
		delete[] tmpadresN;
		return false;
	}
	else{
		pidsN.resize(idx+1);
		chidsN.resize(idx+1);
		pidsN[idx]=pidN;
		chidsN[idx]=chidN;
		zmq::message_t requestN (5);
		memcpy (requestN.data (), "Ready", 5);
		socketsN[idx].send (requestN,zmq::send_flags::none);
		zmq::message_t replyN;
		socketsN[idx].recv(replyN,zmq::recv_flags::none);
//		printf("child: %s\n",(char*)replyN.data());
		return true;
	}
	printf("something failed\n");
	return false;
}

bool checkTCP(char*ard){
	int i;
	for(i=0;i<6;i++){
		if(ard[i]!="tcp://"[i]){
			printf("%s\n",ard);for(int k=0;k<i;k++){printf(" ");}printf("^ %c\n",ard[i]);
			return false;
		}
	}
	for(int j=0;j<4;j++){
		while((ard[i]>='0')&&(ard[i]<='9')){
			i++;
		}
		if((j<3)&&(ard[i]!='.')){
			printf("%s\n",ard);for(int k=0;k<i;k++){printf(" ");}printf("^ %c\n",ard[i]);
			return false;
		}
		if((j==3)&&(ard[i]!=':')){
			printf("%s\n",ard);for(int k=0;k<i;k++){printf(" ");}printf("^ %c\n",ard[i]);
			return false;
		}
		i++;
	}
	while((ard[i]>='0')&&(ard[i]<='9')){
		i++;
	}
	if((ard[i]=='\0')||(ard[i]=='\n')){
		return true;
	}
	printf("%s\n",ard);for(int k=0;k<i;k++){printf(" ");}printf("^ %c\n",ard[i]);
	return false;
}

int main( int argc, char *argv[] ){
	printf("arguments:\n");
	for(int i=0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
	printf("arguments over\n");

	zmq::context_t context (1);
	zmq::socket_t psocket (context, ZMQ_REP);
	psocket.connect (argv[1]);
	zmq::message_t reply;
	psocket.recv(reply,zmq::recv_flags::none);
//	printf("parent: %s\n",(char*)reply.data());
	if( argc != 3 ) {
		zmq::message_t request (8);
		memcpy (request.data (), "BadInit\0", 8);
		psocket.send(request,zmq::send_flags::none);
		psocket.close();
		context.close();
		exit(-1);
	}
	if(!checkTCP(argv[1])){
		zmq::message_t request (7);
		memcpy (request.data (), "BadTCP\0",7);
		psocket.send(request,zmq::send_flags::none);
		psocket.close();
		context.close();
		exit(-1);
	}
	int CurId=0;
	if(argv[2][0]=='M'){CurId=-1;}
	else{
		for(int i=(argv[2][0]=='-');argv[2][i]!='\0';i++){
			CurId=CurId*10+argv[2][i]-'0';
		}
		if(argv[2][0]=='-'){
			CurId*=-1;
		}
	}
	zmq::message_t request (6);
	memcpy (request.data (), "Ready\0", 6);
printf("got here safely\n");
	psocket.send(request,zmq::send_flags::none);
printf("answered to parent\n");
	char*NodeExec=new char(10);
	for(int i=0;i<10;i++){
		NodeExec[i]="./nodo.out"[i];
	}
	std::vector<pid_t> pids={};
	std::vector<int> chids={};
	int num=255,i=-1,j,ln,tln;
	std::map<std::string,int>vocab{};
	int type,arg1=-8,arg2=-8,idx;
	std::string command,arg3="\0dummy\0";
	bool found,success;
	int bdr1,bdr2;
	char*inpt=(char*)malloc(sizeof(char)*num);
	std::vector<zmq::socket_t> sockets={};
	int heartbit=0;


	printf("entering work loop\n");

	double heartBitT=0;
	clock_t pasT=clock(),curT=pasT;
	bool fwd=true;
	while(i!=6){
//listen to your heart
/*		if(heartbit){
			curT = clock();
			heartBitT += (double)(curT - pasT);
			pasT = curT;
			if(heartBitT > (double)(heartbit/1000 * CLOCKS_PER_SEC))
				{
				heartBitT =0;//-= (double)(heartbit/1000 * CLOCKS_PER_SEC);
				printf("%d Thump\n", CurId);
			}
		}*/
//listen to the rain


//		psocket.recv (reply,zmq::recv_flags::none);
		if((psocket.recv (reply,zmq::recv_flags::dontwait))&&(zmq_errno()==EAGAIN)){
			sockReset(psocket,context);
		}
		else{
//        	printf("child received \"%s\"\n",(char*)reply.data());
		command=(char*)reply.data();
//		std::cout<<CurId<<": command received: "<<command<<"\n";
		i=1;
		arg1=0;
		arg2=0;
		arg3="+";
		while(command[i]==' '){i++;}
		if(command[i]=='-'){arg3="-";i++;}
		while(command[i]!=' '){
			arg1=arg1*10+((char)command[i])-'0';
			i++;
		}
		if(arg3=="-"){arg1*=-1;arg3="+";}
		while(command[i]==' '){i++;}
		if(command[i]=='-'){arg3="-";i++;}
		while(command[i]!=' '){
			arg2=arg2*10+((char)command[i])-'0';
			i++;
		}
		if(arg3=="-"){arg2*=-1;}
		arg3="";
		while(command[i]==' '){i++;}
		while(command[i]!='\0'){
			arg3=arg3+((char)command[i]);
			i++;
		}
		i=((char)command[0])-'0';


		//we do a massive amount of trolling here
//		printf("@ %d: ",CurId);
		fwd=true;
		success=true;
		if((i==1)&&(arg2==CurId)){
			i=6;
			command[0]=6;
		}
		if(i!=4){
			if((i==0)&&(arg2==CurId)){
				fwd=false;
				success=true;
				for(int che=0;che<chids.size();che++){
					if(arg1==chids[che]){
						success=false;
						printf("node already exists\n");
					}
				}
				if(success){
					if(!newChildNode(pids , chids , sockets , context , NodeExec , arg1, &heartbit)){
						success=false;
						printf("could not create new node\n");
					}
					else{
						printf("created new node\n");
					}
				}
			}
			else if((i==2)&&(arg1==CurId)){
				fwd=false;
				if(vocab.count(arg3)){
					printf("word already exists\n");
				}else{
					vocab[arg3]=arg2;
					success=true;
					printf("word added\n");
				}
			}
			else if((i==3)&&(arg1==CurId)){
				fwd=false;
				if(vocab.count(arg3)){
					success=true;
					printf("word found, will return key\n");
				}else{
					printf("no such word\n");
				}
			}
		}
		if(fwd){
//			printf("forward command\n");
			found=false;
			for(idx=0;idx<chids.size();idx++){
				std::cout<<chids[idx]<<" ";
				if(arg2==chids[idx]){
					found=true;
					break;
				}
			}
			std::cout<<"\n";
			if(found){
				bdr1=idx;
				bdr2=idx+1;
			}
			else{
				bdr1=0;
				bdr2=sockets.size();
			}

			for(idx=bdr1;idx<bdr2;idx++){
				zmq::message_t request (command.size());
				memcpy (request.data (), command.c_str(), command.size());
				sockets[idx].send (request,zmq::send_flags::none);
			}
			zmq::message_t reply;
			for(idx=bdr1;idx<bdr2;idx++){
				sockets[idx].recv(reply,zmq::recv_flags::none);
//				printf("answer %d:%s\n",idx,(char*)reply.data());
				if(((char*)reply.data())[0]=='Y'){
					success=true;
				}
			}
		}
//        	std::cout<<i<<" "<<arg1<<"  "<<arg2<<"  "<<arg3<<"\n";
		zmq::message_t request (4);
		if(success){
			memcpy (request.data (), "Yea\0", 4);
		}
		else{
			memcpy (request.data (), "Nay\0", 4);
		}
		psocket.send(request,zmq::send_flags::none);
		}//ELSE from got_command? is here
	}
	printf("%d exiting\n",CurId);
	psocket.close();
	for(idx=0;idx<sockets.size();idx++){
		sockets[idx].close();
	}
	context.close();
	return 0;
}

//

//0        "create childID parentID"
//1        "remove childID"
//2        "exec childID name value"
//3        "exec childID name"
//4        "heartbit TIME"
//5        "ping childID"
//6        "exit"
