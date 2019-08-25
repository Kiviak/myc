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

//the function prints the information of the interface,like the linux command IFCONFIG
void print_interface_info(int sockid,char *name){
	struct ifreq  oj;

	strcpy(oj.ifr_ifrn.ifrn_name,name);//name is the interface name(like:etho).
	printf("\n----interface name: %s--------------\n",oj.ifr_ifrn.ifrn_name);
	ioctl(sockid,SIOCGIFMTU,&oj); //get mtu of the interface
	printf("mtu:%d\n",oj.ifr_ifru.ifru_mtu);

	ioctl(sockid,SIOCGIFADDR,&oj);//get the ip addr of the interface.
	struct  sockaddr_in *pp=(struct sockaddr_in *)(&oj.ifr_addr);
	printf("ip addr:%s\n",inet_ntoa(pp->sin_addr));

	ioctl(sockid,SIOCGIFBRDADDR,&oj); //get the broadcast addr of the interface.
	printf("broadcast addr:%s\n",inet_ntoa(pp->sin_addr));

	ioctl(sockid,SIOCGIFNETMASK,&oj);//get the subnet mask of the interface.
	printf("subnet mask:%s\n",inet_ntoa(pp->sin_addr));

	ioctl(sockid,SIOCGIFHWADDR,&oj); //get the MAC of the interface.
	unsigned char *pt=&oj.ifr_hwaddr.sa_data;
	printf("hardware addr:%02x:%02x:%02x:%02x:%02x:%02x\n"
,(int)(pt[0]),(int)(pt[1]),(int)(pt[2]),(int)(pt[3]),(int)(pt[4]),(int)(pt[5]));

	printf("--------------------------------------------------\n\n");
}


int main(int argc, char **argv){


        pid_t pid;
        char arr[1000];
        int i,j,fno,sockid;


        sockid=socket(AF_INET,SOCK_STREAM,IPPROTO_TCP);//2 1 6
        if(sockid<0){
                printf("can't creat socket: %s\n",argv[1]);
                exit(-1);
        }

	struct ifconf  mconf;
	int  mlen=100*sizeof(struct ifconf);
	mconf.ifc_len=mlen;
	mconf.ifc_req=(struct ifreq *)malloc(mlen);
	i=ioctl(sockid,SIOCGIFCONF,&mconf); //get list of all interfaces.
	if(i){
		printf("error:%d\n",errno);
		exit(5);
	}
	j=mconf.ifc_len/sizeof(struct ifreq);//get the number of the interfaces.
	for(i=0;i<j;i++){
		print_interface_info(sockid,mconf.ifc_req[i].ifr_name);//printf each interface info.
	}
	
        printf("main is done\n");
        exit(0);
}

                                                                   
