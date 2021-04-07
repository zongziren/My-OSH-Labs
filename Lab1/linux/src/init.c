#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/sysmacros.h>
#include <sys/wait.h>

int main() 
{
    int status;
    pid_t fpid1,fpid2,fpid3;
    printf("Begin!\n");
    if (mknod("/dev/ttyS0", S_IFCHR | S_IRUSR | S_IWUSR, makedev(4, 64)) == -1) 
    {
        perror("mknod() failed");
    }
    if (mknod("/dev/ttyAMA0", S_IFCHR | S_IRUSR | S_IWUSR, makedev(204, 64)) == -1) 
    {
        perror("mknod() failed");
    }
    if (mknod("/dev/fb0", S_IFCHR | S_IRUSR | S_IWUSR, makedev(29, 0)) == -1) 
    {
        perror("mknod() failed");
    }
    if((fpid1=fork())==0)
    { 
        char * argv1[] = {"1",NULL};
        execv("/1",argv1); 
    }
    else
    {
        waitpid(fpid1, &status, 0 );
        if((fpid2=fork())==0)
        {    
            char * argv2[] = {"2",NULL};
            execv("/2",argv2);
        }
        else
        {
            waitpid(fpid2, &status, 0 );
            if((fpid3=fork())==0)
            {    
                char * argv2[] = {"3",NULL};
                execv("/3",argv2);
            }
            else
            {
                waitpid(fpid3, &status, 0 );
            }
        }
    }
    pause();
    return 0;
}
