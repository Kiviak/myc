#include<stdio.h>
#include<string.h>
#include<sys/stat.h>
#include<unistd.h>
#include<stdlib.h>
#include<signal.h>
#include<errno.h>

void file_type(struct stat file_stat,char *type)
{
    char *p=NULL;
    if(S_ISREG(file_stat.st_mode)){
        p="regular file";
    }else if (S_ISDIR(file_stat.st_mode))
    {
        p="dir file";
    }else if (S_ISLNK(file_stat.st_mode))
    {
        p="symbolic link";
    }else if (S_ISCHR(file_stat.st_mode))   
    {
        p="character special file";
    }else if (S_ISBLK(file_stat.st_mode))    
    {
        p="block special file";
    }else if (S_ISFIFO(file_stat.st_mode))    
    {
        p="pipe or FIFO";
    }else if (S_ISSOCK(file_stat.st_mode))    
    {
        p="socket";
    }

    strcpy(type,p);
      
}

void fun1(){
    printf("from fun1\n");
}

void fun2(){
    printf("from fun2\n");
}

void alarm_sig(int k){
    printf("catch:%d\n",k);
}
void sigf2(int k, siginfo_t * info, void *pp){
    printf("signal catch:%d\n",k);
    printf("info value:%d\n",(*info).si_value);

}

#define MYSIG SIGRTMIN+2

int main(int argc,char* argv[])
{
    char *file_path=argv[1];
    struct stat file_stat;
 
    
    printf("main run\n");
 
    struct sigaction sgg;
    sgg.sa_handler=alarm_sig;
    sgg.sa_flags=SA_SIGINFO;
    sgg.sa_sigaction=sigf2;
    sigaction(MYSIG,&sgg,NULL);

    
    sigset_t mset,old;
    sigemptyset(&mset);
    sigemptyset(&old);
    sigaddset(&mset,MYSIG);
    sigaddset(&mset,SIGTSTP);
    sigaddset(&mset,11);
    // sigprocmask(SIG_BLOCK,NULL,&old);
    sigprocmask(SIG_BLOCK,&mset,NULL);
    // raise(MYSIG);
    // raise(MYSIG);
    union sigval io;
    io.sival_int=67;
    sigqueue(getpid(),MYSIG,io);
    sigqueue(getpid(),MYSIG,io);
    sigqueue(getpid(),MYSIG,io);
    sigprocmask(SIG_UNBLOCK,&mset,NULL);

    int in=sigismember(&old,MYSIG);
    if(in){
        printf("signal is in\n");
    }


    printf("pid:%d\n",getpid());
    printf("ppid:%d\n",getppid());
    printf("uid:%d\n",getuid());
    printf("euid:%d\n",geteuid());
    printf("gid:%d\n",getgid());
    printf("egid:%d\n",getegid());

    
    atexit(fun1);
    atexit(fun2);
    atexit(fun1);
    atexit(fun2);

    return(0);
    
}
