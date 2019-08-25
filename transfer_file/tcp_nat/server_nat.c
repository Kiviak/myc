#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include<unistd.h>
#include<pthread.h>
#include<sys/select.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/wait.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>



int main(int argc, char **argv){
	
	if(argc!=3){
		printf("This is a SERVER based on tcp for transferring files\n\
to a local host.\n\
use it like this: ./server [server'ip] [file'path]\n");
		exit(-1);
	}
	
	int fno,i,j,sockid;
	char arr[1000];
	
	fno=open(argv[2],O_RDONLY);
	if(fno==-1){
		printf("can't open the file:%s\n",argv[2]);
		exit(-1);
	}
	struct sockaddr_in saddr,daddr;
	socklen_t len;
	saddr.sin_family=2;
	saddr.sin_port=htons(5000);
	inet_aton(argv[1],&saddr.sin_addr);
	daddr=saddr;
	
	sockid=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//2 1 6
	if(sockid<0){
		printf("can't creat socket\n");
		exit(-1);
	}
	j=bind(sockid,&saddr,sizeof(saddr));
	if(j){
		printf("can't bind\n");
		exit(-2);
	}
	j=listen(sockid,6);	
	if(j){
		printf("can't listen\n");
		exit(-3);
	}
	
	int link=accept(sockid,&daddr,&len);
	if(link>-1){
		struct sockaddr_in laddr;
		len=sizeof(laddr);
		getpeername(link,&laddr,&len);
		printf("peer socket:%s:%d\n",inet_ntoa(laddr.sin_addr),ntohs(laddr.sin_port));
		while((j=read(fno,arr,1000))>0)
			write(link,arr,j);
	}
	close(fno);
	close(link);
				
	printf("main is done\n");
	exit(0);
}
			
