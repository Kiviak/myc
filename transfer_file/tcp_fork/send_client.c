#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include<unistd.h>
#include<sys/select.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#define SUM 1


int main(int argc, char **argv){
	
	
	if(argc<3){
		printf("This is a CLIENT based on tcp for transferring files.\n\
use it like this: ./client [server'ip] [server'port] [file'path] [file'path] ...\n\
ps: the server'port is usually 5000.\n");
		exit(-1);
	}

	pid_t pid;
	char arr[1000];
	int i,j,fno,sockid;
	
	struct sockaddr_in saddr,daddr;
	daddr.sin_family=2;
	daddr.sin_port=htons(atoi(argv[2]));
	inet_aton(argv[1],&daddr.sin_addr);
	saddr=daddr;
	
	for(i=3;i<argc;i++){
		fno=open(argv[i],O_RDONLY);
		if(fno==-1){
			printf("can't open file: %s\n",argv[i]);
			continue;
		}
		if((pid=fork())==0){
			sockid=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//2 1 6
			if(sockid<0){
				close(fno);
				printf("can't creat socket: %s\n",argv[i]);
				exit(-1);
			}
			saddr.sin_port=htons(1023+getpid());
			inet_aton("0",&saddr.sin_addr);
			j=bind(sockid,&saddr,sizeof(saddr));
			if(j<0){
				close(fno);
				printf("can't bind: %s\n",argv[i]);
				exit(-2);
			}
			j=connect(sockid,&daddr,sizeof(daddr));
			if(j<0){
				close(fno);
				printf("can't connect: %s\n",argv[i]);
				exit(-3);
			}
			while((j=read(fno,arr,1000))>0){
				j=write(sockid,arr,j);
			}
			close(fno);
			close(sockid);
			printf("trans is done: %s\n",argv[i]);
			exit(0);
		}
	}
	
	printf("client is done\n");
	exit(0);
}
