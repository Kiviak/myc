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
#include<arpa/inet.h>


void * thread_main(void *);
struct thread_args{
	int link;
};

struct sockaddr_in saddr,daddr;

int main(int argc, char **argv){
	
	if(argc!=2){
		printf("This is a SERVER based on tcp for transferring files.\n\
use it like this: ./server [server'ip]\n");
		exit(-1);
	}
	
	int i,j,sockid;
	saddr.sin_family=2;
	saddr.sin_port=htons(5000);
	inet_aton(argv[1],&saddr.sin_addr);
	
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
	j=listen(sockid,100);	
	if(j){
		printf("can't listen\n");
		exit(-3);
	}
	
	fd_set mset,dset;
        struct timeval mtimes;
    	mtimes.tv_sec=20;
    	mtimes.tv_usec=0;
    	FD_ZERO(&mset);
    	FD_ZERO(&dset);    
	FD_SET(sockid,&dset);
	int max=sockid;
	
	while(1){
		mset=dset;
		j=select(max+1,&mset,NULL,NULL,&mtimes);
		if(j>0){
			if(FD_ISSET(sockid,&mset)){
				socklen_t len=sizeof(daddr);
				int link=accept(sockid,&daddr,&len);
				if(link>-1){
					struct thread_args *pt=(struct thread_args *)malloc(sizeof(struct thread_args));
					pt->link=link;
					pthread_t mthd;
					pthread_create(&mthd,NULL,thread_main,pt);
					//close(link);
				}
			}
		}
	}
	
	printf("main is done\n");
	exit(0);
}
void *thread_main(void *pt){
	struct thread_args mpt;
	memcpy(&mpt,pt,sizeof(struct thread_args));
	struct thread_args *tt=(struct thread_args *)pt;
	free(tt); 
	struct sockaddr_in laddr;
	socklen_t len=sizeof(laddr);
	getpeername(mpt.link,&laddr,&len);
	printf("peer socket:%s:%d\n",inet_ntoa(laddr.sin_addr),ntohs(laddr.sin_port));
	char path[40];
	char randc[20];
	strcpy(path,"./");
	sprintf(randc,"%d",pthread_self());
	strcat(path,"temp");
	strcat(path,randc);
	int j,fno;
	char arr[1000];
	fno=creat(path,0600);
	if(fno==-1){
		printf("can't creat the file:%s\n",path);
		close(mpt.link);
		pthread_exit(NULL);
	}
	while((j=read(mpt.link,arr,1000))>0)
		write(fno,arr,j);
	close(fno);
	close(mpt.link);
	printf("%s is done\n",path);
	pthread_exit(NULL);
}
