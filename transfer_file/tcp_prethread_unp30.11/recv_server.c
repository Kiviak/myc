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
	int listen_sock;
};

struct sockaddr_in saddr,daddr;
pthread_mutex_t mlock;

int main(int argc, char **argv){
	
	if(argc!=2){
		printf("This is a SERVER based on tcp for transferring files.\n\
use it like this: ./server [server'ip]\n");
		exit(-1);
	}
	
	pthread_mutex_init(&mlock,NULL);
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
	pthread_t thid;
	struct thread_args *pt=(struct thread_args *)malloc(sizeof(struct thread_args));
	pt->listen_sock=sockid;
	for(i=0;i<5;i++){
		pthread_create(&thid,NULL,thread_main,pt);
		printf("create a thread:%d\n",thid);
	}

	while(1)
		sleep(20);
	free(pt);
	printf("main is done\n");
	exit(0);
}
void *thread_main(void *pt){
	struct thread_args mpt;
	memcpy(&mpt,pt,sizeof(struct thread_args));
	struct sockaddr_in laddr;
	socklen_t len=sizeof(laddr);
	
	char path[40],path2[40];
	char randc[20];
	strcpy(path,"./");
	sprintf(randc,"%d",pthread_self());
	strcat(path,"temp_");
	strcat(path,randc);
	strcat(path,"_");
	strcpy(path2,path);
	int j,fno,beg=0;
	char arr[1000];

	while(1){
		pthread_mutex_lock(&mlock);
		int link=accept(mpt.listen_sock,&laddr,&len);
		pthread_mutex_unlock(&mlock);
		if(link==-1){
			printf("can't acceept,thread id:%d\n",pthread_self());
			continue;
		}
		len=sizeof(laddr);
		getpeername(link,&laddr,&len);
		printf("thread is:%d .peer socket:%s:%d\n",pthread_self(),inet_ntoa(laddr.sin_addr),ntohs(laddr.sin_port));
		strcpy(path,path2);
		sprintf(randc,"%d",beg++);
		strcat(path,randc);
		fno=creat(path,0600);
		if(fno==-1){
			printf("can't creat the file:%s\n",path);
			close(link);
			continue;
		}
		while((j=read(link,arr,1000))>0)
			write(fno,arr,j);
		close(fno);
		close(link);
		printf("%s is done\n",path);
	}
}
