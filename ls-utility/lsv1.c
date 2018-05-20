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
    // reading content of pwd/cwd directory
    if(argc==1)
    {
        do_ls(".");
    }
    else
    {
        // printing content of given directory
        int i = 0;
        while(++i<argc)
        {
            printf("Directory listing of %s:\n",argv[i]);
            do_ls(argv[i]);
        }
    }
    exit(0);
}
void do_ls(char* dir)
{
    DIR* dp = opendir(dir);
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
          return;
        }
        else
        {
            printf("%s\n",entry->d_name);
        }
    }
    closedir(dp);
}
