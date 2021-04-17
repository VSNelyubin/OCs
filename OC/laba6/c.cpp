#include <iostream>


#include <chrono>
#include <thread>


#include <string>
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
	printf("\n");
	return rez;
}

const int CMNM=6;
const size_t TCPE=32;

bool newChildNode(std::vector<pid_t> &pidsN , std::vector<int> &chidsN , std::vector<zmq::socket_t> &socketsN , zmq::context_t&contextN , char*NodeExecN,int chidN){
	int idx=socketsN.size();
	socketsN.resize(idx+1);
	socketsN[idx]=zmq::socket_t(contextN,ZMQ_REQ);
//	zmq::socket_t socketN (contextN, ZMQ_REQ);
        socketsN[idx].bind ("tcp://*:*");//*/
        char*tmpadresN=new char[TCPE];
        size_t tmpsizeN=TCPE*sizeof(char);
        int fqN;
        fqN=zmq_getsockopt(socketsN[idx],ZMQ_LAST_ENDPOINT,tmpadresN,&tmpsizeN);
	if(fqN){
		socketsN[idx].close();
		socketsN.resize(idx);
                delete[] tmpadresN;
		printf("socket ID failure %d\n",fqN);
                return false;
	}
	pid_t pidN;
//	printf("will fork\n");
	pidN=fork();
	if(pidN==0){
//		printf("child here\n");
		std::string vtmp=std::to_string(chidN)+"\0\n";
		char*chidAr=new char[16];
		for(int chi=0;vtmp[chi]!='\n';chi++){
			chidAr[chi]=vtmp[chi];
		}
		char*args[]={NodeExecN,tmpadresN,NULL};
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
//		std::chrono::seconds dura( 5);
//		std::this_thread::sleep_for( dura );

//		printf("parent continues\n");
		pidsN.push_back(pidN);
		chidsN.push_back(chidN);
//		printf("will send message\n");
		zmq::message_t requestN (5);
//		printf("will memcpy\n");
		memcpy (requestN.data (), "Ready", 5);
//		printf("sending...\n");
		socketsN[idx].send (requestN,zmq::send_flags::none);
//		printf("message sent. awaiting response\n");
		zmq::message_t replyN;
		socketsN[idx].recv(replyN,zmq::recv_flags::none);
		printf("child: %s\n",(char*)replyN.data());
		return true;
	}
	printf("something failed\n");
	return false;
}

int main(){

	char*NodeExec=new char(10);
        for(int i=0;i<10;i++){
                NodeExec[i]="./node.out"[i];
        }

	std::vector<pid_t> pids={};
	std::vector<int> chids={};
	int num=255,i,j,ln,tln;

	std::map<std::string,int>vocab{};

	int type,arg1=-8,arg2=-8,idx;
	std::string command,arg3="\0dummy\0";

	bool found,success;
	int bdr1,bdr2;

	int CurId=-1;

	const char*cmds[CMNM]={"create","remove","exec","heartbit","ping","exit"};
	char*inpt=(char*)malloc(sizeof(char)*num);

	zmq::context_t context (1);
        std::vector<zmq::socket_t> sockets={};// (context, ZMQ_REP);

	while(NULL!=fgets (inpt, num, stdin)){
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
        	j++;
        	if(!found){
            		printf("Error: command not found\n");
            		continue;
        	}
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

		command=std::to_string(i)+" "+std::to_string(arg1)+" "+std::to_string(arg2)+" "+arg3+"\0";

		//the segment after this is should be node code only
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
		success=false;
		if((i==2)&&(arg2==CurId)){
			i=6;
			command[0]=6;
		}
		if(i!=4){
			if((i==0)&&(arg2==CurId)){
				if(!newChildNode(pids , chids , sockets , context , NodeExec , arg1)){
					printf("could not create new node\n");
				}
				else{
					success=true;
				}
			}
			else if((i==2)&&(arg1==CurId)){
				if(vocab.contains(arg3)){
					vocab[arg3]=arg2;
					success=true;
				}
			}
			else if((i==3)&&(arg1==CurId)){
                                if(vocab.contains(arg3)){
                                        success=true;
                                }

                        }
			else{
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
				}//create was determined
			}
		}

				for(idx=bdr1;idx<bdr2;idx++){
					zmq::message_t request (command.size());
					memcpy (request.data (), command.c_str(), command.size());
		                	sockets[idx].send (request,zmq::send_flags::none);
				}

                		zmq::message_t reply;
				for(idx=bdr1;idx<bdr2;idx++){
					sockets[idx].recv(reply,zmq::recv_flags::none);
                			printf("answer %d:%s\n",idx,(char*)reply.data());
				}
			}
		}
        	std::cout<<i<<" "<<arg1<<"  "<<arg2<<"  "<<arg3<<"\n";
    	}
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
