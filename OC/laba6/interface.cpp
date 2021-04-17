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

int getIntM(char*str,int&offs){
	int rez=0,sig=1;
	if(str[offs]=='-'){
		sig=-1;
		offs++;
	}
	for(;((str[offs]>='0')&&(str[offs]<='9'));offs++){
		rez=rez*10+str[offs]-'0';
	}
	rez*=sig;
	return rez;
}

std::string getStrM(char*str,int&offs){
	std::string rez="";
	for(;((str[offs]!='\0')&&(str[offs]!=' ')&&(str[offs]!='\n'));offs++){
//        	printf("%c ",str[offs]);
		rez=rez+str[offs];
	}
//	printf("\n");
	return rez;
}

double getDubM(char*str,int&offs){
	double rez=0;
	int sig=1;
	if(str[offs]=='-'){
		sig=-1;
		offs++;
	}
	for(;((str[offs]>='0')&&(str[offs]<='9'));offs++){
		rez=rez*10+str[offs]-'0';
	}
	if(str[offs]!='.'){
		rez*=sig;
		return rez;
	}
	int tenpow=0;
	offs++;
	for(;((str[offs]>='0')&&(str[offs]<='9'));offs++){
		rez=rez*10+str[offs]-'0';
		tenpow++;
	}
	while(tenpow){
		rez*=0.1;
		tenpow--;
	}
	rez*=sig;
	return rez;
}

const int CMNM=6;
const size_t TCPE=32;
//void waitForMessage(zmq::socket_t &socket, zmq::message_t &reply, int &flag){
void updateTimers(std::map<int,double>&timers,zmq::socket_t &socket){
	zmq::message_t reply;
	int chid,i;
	double pulse;
	while(1){
printf("qwe\n");
		while(!((socket.recv (reply,zmq::recv_flags::dontwait))&&(zmq_errno()==EAGAIN))){
//printf("awe\n");
			i=0;
			chid=getIntM(((char*)reply.data()),i);
			i++;
			pulse=getDubM(((char*)reply.data()),i);
			timers[chid]=pulse;
		}
	}
}

int main(){
	std::map<int,double>timers;
	std::vector<int>chids={-1};
	char*NodeExec=new char(10);
	for(int i=0;i<10;i++){
		NodeExec[i]="./nodo.out\0"[i];
	}
	int num=255,i,j,ln,tln;
	int type,arg1=-8,arg2=-8,idx;
	std::string command,arg3="\0dummy\0";
	bool found,success;
	int bdr1,bdr2;
	const char*cmds[CMNM]={"create ","remove ","exec ","heartbit ","ping ","exit"};
	char*inpt=(char*)malloc(sizeof(char)*num);
	zmq::context_t context (1);
	zmq::socket_t socket (context,ZMQ_REQ);
	socket.bind ("tcp://*:*");//*/
	char*tmpadresN=new char[TCPE];
	size_t tmpsizeN=TCPE*sizeof(char);

	zmq::socket_t socketT (context,ZMQ_PULL);
	socketT.bind ("tcp://*:*");//*/
	char*heartbitListener=new char[TCPE];
	if(zmq_getsockopt(socketT,ZMQ_LAST_ENDPOINT,heartbitListener,&tmpsizeN)){
		socketT.close();
		delete[] tmpadresN;
		printf("socket T ID failure\n");
		return -1;
	}
/*	if(zmq_setsockopt(socketT,ZMQ_LAST_ENDPOINT,heartbitListener,&tmpsizeN)){
		socketT.close();
		delete[] tmpadresN;
		printf("socket T ID failure %d\n",fqN);
		return -1;
	}
*/

	tmpsizeN=TCPE*sizeof(char);
	int fqN;
	fqN=zmq_getsockopt(socket,ZMQ_LAST_ENDPOINT,tmpadresN,&tmpsizeN);
	if(fqN){
		socket.close();
		delete[] tmpadresN;
		printf("socket C ID failure %d\n",fqN);
		return -1;
	}
//printf("child tcp %s\n timer tcp %s\n",tmpadresN,heartbitListener);
	pid_t pidN;
//printf("fork<\n");
	pidN=fork();
	if(pidN==0){
//		printf("child fork\n");
		char*chidAr=new char[2];
		chidAr[0]='M';
		chidAr[1]='\n';
//		printf("what?\n");
		char*args[]={NodeExec,tmpadresN,chidAr,heartbitListener,NULL};
//		for(int ii=0;ii<2;ii++){
//			printf("%s\n",args[ii]);
//		}
		execvp(args[0],args);
		_exit (EXIT_FAILURE);
	}
	else if(pidN<0){
		printf("something went wrong with fork()\n");
		socket.close();
		delete[] tmpadresN;
		return false;
	}
	else{
//		printf("forked\n");
	}
	zmq::message_t requestN (5);
	memcpy (requestN.data (), "Ready", 5);
//printf("PLESE\n");
	socket.send (requestN,zmq::send_flags::none);
	zmq::message_t replyN;
//printf("receive childready\n");
	socket.recv(replyN,zmq::recv_flags::none);//dontwait);
//printf("received safely\n");
//	printf("root: %s; err=%d\n",(char*)replyN.data(),errno);

	bool snd,needEnd=true;

	std::thread lifeChecker;

	int heartbit=0;

//	printf("THE FUCK?\n");

	lifeChecker=std::thread(updateTimers,std::ref(timers),std::ref(socket));

	while(NULL!=fgets (inpt, num, stdin)){
		needEnd=false;
		ln=(int)strlen(inpt);
		if(inpt[0]=='\n'){
			break;
		}
		for(i=0;i<CMNM;i++){
			found=true;
			tln=(int)strlen(cmds[i]);
			if(ln<tln){continue;}
			for(j=0;j<tln;j++){
				if(cmds[i][j]!=inpt[j]){
					found=false;
					break;
				}
			}
			if(found){
				break;
			}
			found=false;
		}
//	        j++;
		if(!found){
			printf("Error: command not found\n");
			continue;
		}
//		printf("%s\n",inpt);
//for(int ru=0;ru<j;ru++){printf(" ");}printf("^ %c\n",inpt[j]);
		if(i!=6){
			arg1=getIntM(inpt,j);
			if(i>2){i++;}
			if(i==0){
				j++;
				arg2=getIntM(inpt,j);
			}
			if(i==2){
				j++;
				arg3=getStrM(inpt,j);
				if(inpt[j]==' '){
					j++;
					arg2=getIntM(inpt,j);
				}
				else{i++;}
			}
		}
		snd=true;
		if(i==0){
			for(tln=0;tln<chids.size();tln++){
				if(chids[tln]==arg1){
					printf("Error: Already exists\n");
					snd=false;
					break;
				}
				if(chids[tln]==arg2){
					snd=true;//parent found
					break;
				}
			}
			if(tln==chids.size()){
				printf("Error: Parent not found\n");
				snd=false;
			}
		}
		if(((i>0)&&(i<4))||(i==5)){
			for(tln=0;tln<chids.size();tln++){
				if((chids[tln]==arg1)){
					snd=true;//node found
					break;
				}
			}
			if(tln==chids.size()){
				printf("Error: Not found\n");
				snd=false;
			}
		}
		if(i==5){
			snd=false;
			if(heartbit>0){
				if(((double)clock()-timers[arg1]) > 4*(double)(heartbit/1000 * CLOCKS_PER_SEC)){
					printf("Ok: 0\n");
				}
				else{
					printf("Ok: 1\n");
				}
			}
			else{
				printf("Error: bad heartbit value. use heartbit [MILLISECOND COUNT] command to set pulse rate\n");
			}
		}

		if(snd){
			command=std::to_string(i)+" "+std::to_string(arg1)+" "+std::to_string(arg2)+" "+arg3+"\0";
//		std::cout<<"sending command \""<<command<<"\"\n";
			zmq::message_t request (command.size());
			memcpy (request.data (), command.c_str(), command.size());
			socket.send(request,zmq::send_flags::none);
			zmq::message_t reply;
			socket.recv(reply,zmq::recv_flags::none);
			printf("main answer: %s\n",(char*)reply.data());
//char tpm;for(int jj=0;true;jj++){tpm=((char*)reply.data())[jj];if(tpm=='\0'){printf("\\0");break;}else if(tpm=='\n'){printf("\\n");}else if(tpm==' '){printf("_");}else{printf("%c",tpm);}}printf("\n");
			if((i==0)&&(((char*)reply.data())[0]=='Y')){
				chids.push_back(arg1);
			}
			if((i==1)&&(((char*)reply.data())[0]=='Y')){
				int ii;
				for(ii=0;chids[ii]!=arg1;ii++);
				for(ii;ii<chids.size()-1;ii++){
					chids[ii]=chids[ii+1];
				}
				chids.resize(chids.size()-1);
			}
			if(((char*)reply.data())[0]=='K'){
				int ii=1,jj=1,ji=1;
char tpm;
				while(jj<reply.size()){
					jj++;
tpm=((char*)reply.data())[jj];//if(tpm=='\0'){printf("\\0");break;}else if(tpm=='\n'){printf("\\n");}else if(tpm==' '){printf("_");}else{printf("%c",tpm);}
					if((tpm>'9')||(tpm<'0')){break;}
					ji=getIntM((char*)reply.data(),jj);
//printf("%d is deleted\n",ji);
					for(ii=0;chids[ii]!=ji;ii++);
					for(ii;ii<chids.size()-1;ii++){
						chids[ii]=chids[ii+1];
					}
					chids.resize(chids.size()-1);
				}
			}
			printf("running processes: ");for(int ii=0;ii<chids.size();ii++){printf("%d ",chids[ii]);}printf("\n");
//			printf("main answer: %s\n",(char*)reply.data());
		}
		if(i==6){
			break;
		}
		if((i==1)&&(arg1==-1)){
			break;
		}
		needEnd=true;
	}
	lifeChecker.join();
	if(needEnd){
		command="6 0 0 0\0\0";
		zmq::message_t request (command.size());
		memcpy (request.data (), command.c_str(), command.size());
		socket.send(request,zmq::send_flags::none);
	}
	socket.close();
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
