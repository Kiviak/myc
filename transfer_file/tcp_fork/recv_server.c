#include<stdio.h>
#include<stdlib.h>
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
	
	if(argc!=2){
		printf("This is a SERVER based on tcp for transferring files.\n\
use it like this: ./server [server'ip]\n");
		exit(-1);
	}
	
	int beg=0;
	int link_sum=0;
	int fno,i,j,sockid;
	int mpoll[10];
	int dict[100];
	char mpath[40];
	char randc[20];
	char arr[1000];
	for(i=0;i<10;i++)
		mpoll[i]=-1;
	
	struct sockaddr_in saddr,daddr;
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
			if(FD_ISSET(sockid,&mset)&&link_sum<10){
				socklen_t len=sizeof(daddr);
				int link=accept(sockid,&daddr,&len);
				if(link>-1){
					for(i=0;i<10;i++){
						if(mpoll[i]==-1){
							mpoll[i]=link;
							struct sockaddr_in laddr;
							len=sizeof(laddr);
							getpeername(link,&laddr,&len);
							printf("peer socket:%s:%d\n",inet_ntoa(laddr.sin_addr),ntohs(laddr.sin_port));
							if(link>max)
								max=link;
							FD_SET(link,&dset);
							link_sum++;
							strcpy(mpath,"./");
							sprintf(randc,"%d",beg++);
							strcat(mpath,"temp");
							strcat(mpath,randc);
							j=creat(mpath,0600);
							if(j>-1)
								dict[link]=j;
							break;
						}
				 	}
				}
			}	
			for(i=0;i<10;i++){
				if((mpoll[i]!=-1)&&FD_ISSET(mpoll[i],&mset)){
					j=read(mpoll[i],arr,1000);
					if(j>0){
						write(dict[mpoll[i]],arr,j);
					}else{
						close(dict[mpoll[i]]);
						close(mpoll[i]);
						FD_CLR(mpoll[i],&dset);
						mpoll[i]=-1;
						link_sum--;
					} 
				}	
			
			}
		}
	}
				
	printf("main is done\n");
	exit(0);
}
			
