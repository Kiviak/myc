#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<errno.h>
#include<time.h>
#include<unistd.h>
#include<fcntl.h>
#include<pthread.h>
#include<sys/select.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/ioctl.h>
#include<sys/wait.h>
#include<net/if.h>
#include<net/if_arp.h>
#include<net/route.h>
#include<netinet/in.h>
#include<netinet/tcp.h>
#include<arpa/inet.h>



int main(int argc, char **argv){

        if(argc!=2){
                printf("This is a program to print the arp information,like the linux shell command ARP.\n\
use it like this: ./a.out [ip addr]\n");
                exit(-1);
        }

        pid_t pid;
        char arr[1000];
        int i,j,fno,sockid;


        sockid=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//2 1 6
        if(sockid<0){
                printf("can't creat socket: %s\n",argv[1]);
                exit(-1);
        }

	struct arpreq  marp;
	memset(&marp,0,sizeof(marp));

	struct sockaddr_in *ipp=(struct sockaddr_in *)&marp.arp_pa;
	ipp->sin_family=AF_INET;
	ipp->sin_addr.s_addr=inet_addr(argv[1]);

	printf("\nip addr:%s\n",inet_ntoa(ipp->sin_addr));
	ipp=(struct sockaddr_in *)&marp.arp_ha;
	ipp->sin_family=ARPHRD_ETHER;
	strcpy(marp.arp_dev,"eth0");

	ioctl(sockid,SIOCGARP,&marp);
	unsigned char *pt=&marp.arp_ha.sa_data;
	printf("hardware addr:%02x:%02x:%02x:%02x:%02x:%02x\n\n"
,(int)(pt[0]),(int)(pt[1]),(int)(pt[2]),(int)(pt[3]),(int)(pt[4]),(int)(pt[5]));


        close(sockid);
        exit(0);
}

                                                                   
