#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<fcntl.h>
#include"daemonize.h"

typedef void (*pt)(int);
void tes(int key){
    printf("tes:%d\n",key);
}
pt vb(int v,pt fn){
    fn(v);
    return(fn);
}
void (*vb2(int,void (*)(int)))(int);

int main(int argc,char* argv[])
{
    printf("look at\n");
    printf("pid:%d\n",getpid());
    printf("ppid:%d\n",getppid());
    
    daemonize("daemon");

    sleep(180);

    return(0);
}

void (*vb2(int k,void (*h)(int)))(int){
    printf("vb2\n");
    h(k);
    return(h);
}
