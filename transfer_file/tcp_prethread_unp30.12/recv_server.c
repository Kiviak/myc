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

#define MAX_LINK 5
void * thread_main(void *);
struct sockaddr_in saddr,daddr;
pthread_mutex_t mlock;
pthread_cond_t  mcond;
int sum,iget,iput;
int mpoll[MAX_LINK];
sum=0;
iput=0;
iget=0;
int main(int argc, char **argv){
	
	if(argc!=2){
		printf("This is a SERVER based on tcp for transferring files.\n\
use it like this: ./server [server'ip]\n");
		exit(-1);
	}
	
	pthread_mutex_init(&mlock,NULL);
	pthread_cond_init(&mcond,NULL);
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
	for(i=0;i<5;i++){
		pthread_create(&thid,NULL,thread_main,NULL);
		printf("create a thread:%d\n",thid);
	}

	while(1){
		mset=dset;
	    	mtimes.tv_sec=20;
    		mtimes.tv_usec=0;
		j=select(max+1,&mset,NULL,NULL,&mtimes);
		if(j>0){
			socklen_t len;
			len=sizeof(daddr);
			j=accept(sockid,&daddr,&len);
			if(j<0){
				printf("can't accept\n");
				continue;
			}
			pthread_mutex_lock(&mlock);
			if(sum>=MAX_LINK){
				printf("too much link--------------------------------------------------------\n");
				close(j);
				pthread_mutex_unlock(&mlock);
				continue;
			}
			mpoll[iput++]=j;
			if(iput==MAX_LINK)
				iput=0;
			sum++;
			pthread_mutex_unlock(&mlock);
			pthread_cond_signal(&mcond);
		}
	}
	printf("main is done\n");
	exit(0);
}
void *thread_main(void *pt){
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
	int j,fno,link,beg=0;
	char arr[1000];

	while(1){
		pthread_mutex_lock(&mlock);
		while(sum==0)
			pthread_cond_wait(&mcond,&mlock);
		link=mpoll[iget++];
		if(iget==MAX_LINK)
			iget=0;
		sum--;
		pthread_mutex_unlock(&mlock);
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
