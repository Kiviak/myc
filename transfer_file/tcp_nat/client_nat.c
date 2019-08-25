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
                printf("This is a CLIENT based on tcp for a local host(like a NAt host\n\
to receive files from an open server.\n\
use it like this: ./client [server'ip] [server'port]\n\
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

        sockid=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//2 1 6
        if(sockid<0){
                printf("can't creat socket: %s\n",argv[1]);
                exit(-1);
        }
        saddr.sin_port=htons(1023+getpid());
        inet_aton("0",&saddr.sin_addr);
        j=bind(sockid,&saddr,sizeof(saddr));
        if(j<0){
                printf("can't bind: %s\n",argv[1]);
                exit(-2);
        }
        j=connect(sockid,&daddr,sizeof(daddr));
        if(j<0){
                printf("can't connect: %s\n",argv[1]);
                exit(-3);
        }
        char mpath[40];
        char randc[20];
        strcpy(mpath,"./");
        sprintf(randc,"%d",getpid());
        strcat(mpath,"temp");
        strcat(mpath,randc);
        fno=creat(mpath,0600);
        if(fno==-1){
                printf("can't creat the file:%s\n",mpath);
                close(sockid);
                exit(-4);
        }

        while((j=read(sockid,arr,1000))>0)
                j=write(fno,arr,j);
        close(fno);
        close(sockid);

        printf("main is done\n");
        exit(0);
}

                                                                   
