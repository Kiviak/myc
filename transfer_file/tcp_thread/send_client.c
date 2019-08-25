#include<stdio.h>
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
	int fno;
	char path[40];
};

struct sockaddr_in daddr;
int main(int argc, char **argv){
	
	
	if(argc<3){
		printf("This is a CLIENT based on tcp for transferring files.\n\
use it like this: ./client [server'ip] [server'port] [file'path] [file'path] ...\n\
ps: the server'port is usually 5000.\n");
		exit(-1);
	}

	int i,j,fno,sockid;
	
	daddr.sin_family=2;
	daddr.sin_port=htons(atoi(argv[2]));
	inet_aton(argv[1],&daddr.sin_addr);
	
	int sum=0;
	pthread_t ppoll[argc-3];
	for(i=3;i<argc;i++){
		fno=open(argv[i],O_RDONLY);
		if(fno==-1){
			printf("can't open file: %s\n",argv[i]);
			continue;
		}
		pthread_t mthd;
		struct thread_args *pt=(struct thread_args *)malloc(sizeof(struct thread_args));
		pt->fno=fno;
		strcpy(pt->path,argv[i]);
		pthread_create(&mthd,NULL,thread_main,pt);
		ppoll[sum++]=mthd;
	}
	
	for(i=0;i<sum;i++)
		pthread_join(ppoll[i],NULL);
	printf("client is done\n");
	exit(0);
}

void *thread_main(void * pt){
	struct thread_args marg;
	struct thread_args *p=(struct thread_args *)pt;
	memcpy(&marg,pt,sizeof(marg));
	free(p);
	int sockid=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//2 1 6
	if(sockid<0){
		close(marg.fno);
		printf("can't creat socket: %s\n",marg.path);
		pthread_exit(NULL);
	}
	struct sockaddr_in saddr;
	socklen_t  len;
	saddr.sin_family=2;
	saddr.sin_port=htons(1023+pthread_self()+getpid());
	inet_aton("0",&saddr.sin_addr);
	int j=bind(sockid,&saddr,sizeof(saddr));
	if(j<0){
		close(marg.fno);
		printf("can't bind: %s\n",marg.path);
		pthread_exit(NULL);
	}
	j=connect(sockid,&daddr,sizeof(daddr));
	if(j<0){
		close(marg.fno);
		printf("can't connect: %s\n",marg.path);
		pthread_exit(NULL);	
	}
	char arr[1000];
	while((j=read(marg.fno,arr,1000))>0){
		j=write(sockid,arr,j);
	}
	close(marg.fno);
	close(sockid);
	printf("trans is done: %s\n",marg.path);
		
}
	
	
