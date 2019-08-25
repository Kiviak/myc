#include<stdio.h>
#include<unistd.h>
#include<fcntl.h>
#include<stdlib.h>
#include<sys/stat.h>
#include<signal.h>

// 调用此函数可使程序成为守护进程
void daemonize(const char *cmd)
{
    pid_t pid;
    int fd0,fd1,fd2;

    umask(0);

    if ((pid=fork())<0)
    {
        printf("%s:can't fork\n",cmd);
        exit(0);
    }else if (pid!=0)
    {
        exit(0);
    }
    setsid();

    if ((pid=fork())<0)
    {
        printf("%s:can't fork\n",cmd);
        exit(0);
    }else if (pid!=0)
    {
        exit(0);
    }

    if (chdir("/") < 0){
        printf("%s:can't change dir\n",cmd);
        exit(0);
    }

    fd0 = open("/dev/null", O_RDWR);
    fd1 = dup(0);
    fd2 = dup(0);

}