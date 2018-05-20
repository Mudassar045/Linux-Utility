#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
extern int errno;
void do_ls(char*);
int main(int argc, char**argv)
{
    if(argc!=2)
    {
        printf("Please must enter directory name");
        exit(1);
    }
    printf("Directory listing of %s:\n",argv[1]);
    do_ls(argv[1]);
    exit(0);
}
void do_ls(char* dir)
{
    DIR *dp = opendir(dir);
    errno = 0;
    struct dirent* entry;
    if(dp==NULL)
    {
        fprintf(stderr, "Can't open directory: %s",dir);
        exit(errno);
    }
    while(1)
    {
        entry=readdir(dp);
        if(entry!=NULL && errno!=0)
        {
            perror("readdir failed");
            exit(errno);
        }
        if(entry==NULL && errno==0)
        {
          exit(0);
        }
        else
        {
            printf("%s\n",entry->d_name);
        }
    }
    closedir(dp);
}
