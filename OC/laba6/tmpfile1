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

const int CMNM=6;
const size_t TCPE=32;

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

void killChild(std::vector<pid_t> &pidsN , std::vector<int> &chidsN , std::vector<zmq::socket_t> &socketsN,int idx){
	socketsN[idx].close();
	pidsN[idx]=0;
	chidsN[idx]=0;
	if(idx==pidsN.size()){
		pidsN.resize(socketsN.size()-1);
		chidsN.resize(socketsN.size()-1);
		socketsN.resize(socketsN.size()-1);
	}
}

/*bool sockReset(zmq::socket_t &socketN,zmq::context_t&contextN){
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
}*/

bool newChildNode(std::vector<pid_t> &pidsN , std::vector<int> &chidsN , std::vector<zmq::socket_t> &socketsN , zmq::context_t&contextN , char*NodeExecN,int chidN, int*pulse,std::string&str){
	int idx;
	for(idx=0;idx<socketsN.capacity();idx++){
		if((pidsN[idx]==0)&&(chidsN[idx]==0)){
			break;
		}
	}
	if(socketsN.capacity()==idx){
		socketsN.resize(idx+1);
		pidsN.resize(idx+1);
		chidsN.resize(idx+1);
		pidsN[idx]=0;
		chidsN[idx]=0;
	}
	socketsN[idx]=zmq::socket_t(contextN,ZMQ_REQ);
	socketsN[idx].bind("tcp://*:*");//*/
	char*tmpadresN=new char[TCPE];
	size_t tmpsizeN=TCPE*sizeof(char);
	int fqN;
	fqN=zmq_getsockopt(socketsN[idx],ZMQ_LAST_ENDPOINT,tmpadresN,&tmpsizeN);
	if(fqN){
		socketsN[idx].close();
		delete[] tmpadresN;
		printf("socket ID failure %d\n",fqN);
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
		char * writable = new char[str.size() + 1];
		std::copy(str.begin(), str.end(), writable);
		writable[str.size()] = '\0';
		char*args[]={NodeExecN,tmpadresN,chidAr,writable,NULL};
		execvp(args[0],args);
		_exit (EXIT_FAILURE);
	}
	else if(pidN<0){
		printf("something went wrong with fork()\n");
		socketsN[idx].close();
		delete[] tmpadresN;
		return false;
	}
	else{
		pidsN[idx]=pidN;
		chidsN[idx]=chidN;
		zmq::message_t requestN (3);
		memcpy (requestN.data (), "R?\0", 3);
		socketsN[idx].send (requestN,zmq::send_flags::none);
		zmq::message_t replyN;
		socketsN[idx].recv(replyN,zmq::recv_flags::none);
//		printf("new child %d in [%d]: %s\n",chidN,idx,(char*)replyN.data());
		return true;
	}
	printf("something failed\n");
	return false;
}

bool tryToAdd(std::vector<pid_t> &pids , std::vector<int> &chids , std::vector<zmq::socket_t> &sockets , zmq::context_t&context , char*NodeExec,int chid, int*pulse,std::string&heartbitListener){
	bool success=true;
	for(int che=0;che<chids.size();che++){
		if(chid==chids[che]){
			success=false;
			printf("node already exists\n");
		}
	}
	if(success){
		if(!newChildNode(pids , chids , sockets , context , NodeExec , chid, pulse, heartbitListener)){
			success=false;
			printf("could not create new node\n");
		}
		else{
			printf("created new node\n");
		}
	}
	return success;
}

std::string sendToAll(std::vector<pid_t> &pids , std::vector<int> &chids , std::vector<zmq::socket_t> &sockets , int i, int arg1, int arg2, std::string command, int CurId){
	bool found=false;
	int bdr1,bdr2,idx=0;
	if(i==0){
		for(idx=0;idx<chids.capacity();idx++){
			//std::cout<<chids[idx]<<" ";
			if((arg2==chids[idx])&&(pids[idx]!=0)){
				found=true;
				break;
			}
		}
	}
	if((i!=0)&&(i!=4)&&(i!=6)){
		for(idx=0;idx<chids.capacity();idx++){
//			std::cout<<chids[idx]<<" ";
			if((arg1==chids[idx])&&(pids[idx]!=0)){
				found=true;
				break;
			}
		}
	}
	if(found){
		bdr1=idx;
		bdr2=idx+1;
	}
	else{
		bdr1=0;
		bdr2=sockets.capacity();
	}
	bool success=false;
	std::string rez="N\0";
//	std::cout<<CurId<<" will forward "<<command<<" to childs from "<<bdr1<<" to "<<bdr2<<"\n";
	for(idx=bdr1;idx<bdr2;idx++){
		if((pids[idx]!=0)&&(chids[idx]!=0)){
			zmq::message_t request (command.size());
			memcpy (request.data (), (command+"\0").c_str(), command.size()+1);
//			printf("%d forward to %d\n",CurId,chids[idx]);
//			sockets[idx].send (request,zmq::send_flags::none);
//			printf("%d fwd success%d\n",CurId,chids[idx]);
		}
	}
	zmq::message_t reply;
	for(idx=bdr1;idx<bdr2;idx++){

		if((pids[idx]!=0)&&(chids[idx]!=0)){
//                      printf("%d ask%d for answer\n",CurId,chids[idx]);
			sockets[idx].recv(reply,zmq::recv_flags::none);
//			printf("%d got \'%s\'answer from %d\n",CurId,(char*)reply.data(),chids[idx]);
//			printf("answer %d:%s\n",idx,(char*)reply.data());
			if((((char*)reply.data())[0]=='Y')&&(rez=="N\0")){
				rez="Y\0";
				success=true;
			}
			if(((char*)reply.data())[0]=='K'){
//				printf("%s\n",(char*)reply.data());
				rez[0]='K';
				rez+=" ";
				for(int ij=2;((char*)reply.data())[ij]!='\0';ij++){
					rez+=((char*)reply.data())[ij];
				}
//				std::cout<<rez<<"|\n";
				success=true;
				killChild(pids,chids,sockets,idx);
			}
			if(((char*)reply.data())[0]=='V'){
				int val=0;
				for(int ij=1;((char*)reply.data())[ij]!='\0';ij++){
					val=val*10+((char*)reply.data())[ij]-'0';
				}
				rez="V"+std::to_string(val)+"\0";
			}
		}
	}
	return rez;
}

bool addWord(std::map<std::string,int>&vocab,std::string&arg3,int arg2){
	bool success;
	if(vocab.count(arg3)){
		success=false;
	}
	else{
		vocab[arg3]=arg2;
		success=true;
		printf("word added\n");
	}
	return success;
}

bool findWord(std::map<std::string,int>&vocab,std::string&arg3,int&result){
	bool success;
	if(vocab.count(arg3)){
		success=true;
		result=vocab[arg3];
	}
	else{
		success=false;
		result=0;
	}
	return success;
}

void waitForMessage(zmq::socket_t &socket, zmq::message_t &reply, int &flag){
	flag=1;
	socket.recv(reply,zmq::recv_flags::none);
	flag=2;
}

int main( int argc, char *argv[] ){
/*	printf("arguments:\n");
	for(int i=0;i<argc;i++){
		printf("%s\n",argv[i]);
	}
	printf("arguments over\n");
*/	zmq::context_t context (1);
	zmq::socket_t psocket (context, ZMQ_REP);
	psocket.connect (argv[1]);
	zmq::message_t reply;
	psocket.recv(reply,zmq::recv_flags::none);
//	printf("parent: %s\n",(char*)reply.data());
/*	if( argc != 3 ) {
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
*/	int CurId=0;
	while(argv[0][CurId]!='\0'){CurId++;}
	char*NodeExec=new char(CurId+1);
	for(int i=0;i<CurId;i++){
		NodeExec[i]=argv[0][i];
		NodeExec[i+1]='\0';
	}
	CurId=0;
	if(argv[2][0]=='M'){CurId=-1;}
	else{
		for(int i=(argv[2][0]=='-');argv[2][i]!='\0';i++){
			CurId=CurId*10+argv[2][i]-'0';
		}
		if(argv[2][0]=='-'){
			CurId*=-1;
		}
	}

	zmq::socket_t socketT (context, ZMQ_PUSH);
	socketT.connect (argv[3]);

	std::string heartbitListener=argv[3];

	zmq::message_t request (3);//<------------------------------------------------------------------------------------
	memcpy (request.data (), "R!\0", 3);
	psocket.send(request,zmq::send_flags::none);
	std::vector<pid_t> pids={};
	std::vector<int> chids={};
	int num=255,i=-1,j,ln,tln;
	std::map<std::string,int>vocab{};
	int type,arg1=-8,arg2=-8,idx;
	std::string command,answer,arg3="\0dummy\0";
	bool found,success;
	int bdr1,bdr2;
	std::vector<zmq::socket_t> sockets={};
	int heartbit=0;
	int haveMessage=0;//0-thread not running. 1-thread is waiting. 2-ready to join
	std::thread waitMes;
	double heartBitT=0;
//	clock_t pasT=clock(),curT=pasT;
	while(1){
//listen to your heart
		if(heartbit){
			if(((double)clock()-heartBitT) > (double)(heartbit/1000 * CLOCKS_PER_SEC)){
				heartBitT+=(double)(heartbit/1000 * CLOCKS_PER_SEC);//-= (double)(heartbit/1000 * CLOCKS_PER_SEC);
//				std::cout<<CurId<<" thump~ "<<((double)clock()-heartBitT)<<"\n";
				std::string heartbet=std::to_string(CurId)+" "+std::to_string((double)clock())+"\0\0";
				zmq::message_t request (heartbet.length());
				memcpy (request.data (), heartbet.c_str(), answer.length());
				psocket.send(request,zmq::send_flags::dontwait);
				//printf("%d Thump %f\n", CurId,(double)clock()-heartBitT);
			}
		}
//listen to the rain
//listen to the voices in your brain
		if(haveMessage==0){
			haveMessage=1;
			waitMes=std::thread(waitForMessage,std::ref(psocket),std::ref(reply),std::ref(haveMessage));
		}
		if(haveMessage==2){
			haveMessage=0;
			waitMes.join();
//			printf("%d child received \"%s\"\n",CurId,(char*)reply.data());
			command=(char*)reply.data();
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

//			std::cout<<CurId<<" is working with arguments "<<i<<" "<<arg1<<" "<<arg2<<" "<<arg3<<"\n";
			success=true;
			if(i==0){
				if(arg2==CurId){
					success=tryToAdd(pids ,chids ,sockets , context ,NodeExec,arg1, &heartbit,heartbitListener);
				}
				else{
					answer=sendToAll(pids ,chids ,sockets,i,arg1,arg2,command,CurId);
				}
			}
			if((i>0)&&(i<4)){
				if(arg1==CurId){
					if(i==1){
						break;//exit while loop. do all finalization there
						//send message to parent
					}
					if(i==2){
						success=addWord(vocab,arg3,arg2);
					}
					if(i==3){
						int rezu=0;
						success=findWord(vocab,arg3,rezu);
						answer="V"+std::to_string(rezu)+"\0";//std::cout<<"test: "<<answer<<"\n";
					}
				}
				else{
//printf("a\n");
					answer=sendToAll(pids ,chids ,sockets,i,arg1,arg2,command,CurId);
//std::cout<<answer<<"\n";
				}
			}
			if(i==4){
				heartbit=arg1;
				heartBitT=(double)clock();
				answer=sendToAll(pids ,chids ,sockets,i,arg1,arg2,command,CurId);
			}
			if(i==5){
				printf("NODE WAS ASKED FOR PING. USE THE CHEATY HEARTBEAT COLLECTOR\n");
			}
			if(i==6){
				break;//exit while loop. do all finalization there
			}
			if((answer[0]!='V')&&(answer[0]!='K')){
				if(success){
					answer="Y\0";
				}
				else{
					answer="N\0";
				}
			}
			zmq::message_t request (answer.length()+1);
//			std::cout<<"message size is "<<request.size()<<", should be %ld\n",answer.length()+1);
			memcpy (request.data (), answer.c_str(), answer.length()+1);
			psocket.send(request,zmq::send_flags::none);
		}//ELSE from got_command? is here
	}
//	printf("%d exiting\n",CurId);
	answer=sendToAll(pids ,chids ,sockets,6,0,0,"6 0 0 0",CurId);
//	std::cout<<CurId<<" got deletion answer "<<answer<<"\n";
//	std::cout<<"check "<<('\0'==answer[answer.size()])<<"\n";
	if(answer[0]=='K'){
//		answer=answer.substr(0, answer.size()-1);
		answer=answer+" "+std::to_string(CurId);
	}
	else{
		answer="K "+std::to_string(CurId);
	}
//	std::cout<<CurId<<" deletion report: "<<answer<<"\n";
	answer+="\0";
	request=zmq::message_t(answer.size()+1);
	//printf("message size is %lu, should be %ld\n",request.size(),answer.length()+1);
	memcpy (request.data (), answer.c_str(), answer.size()+1);
	psocket.send(request,zmq::send_flags::none);
	psocket.close();
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
