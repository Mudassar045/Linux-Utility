#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <errno.h>
extern int errno;
void do_ls(char*);
int getDirContentCount(char*);
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
    int dirCount = getDirContentCount(dir);
    char** dirContentArr = (char**)malloc(sizeof(char*)*dirCount);
    DIR* dp = opendir(dir);
    errno = 0;
    struct dirent* entry;
    if(dp==NULL)
    {
        fprintf(stderr, "Can't open directory: %s",dir);
        exit(errno);
    }
    int i = 0;
    int maxLen = 0;
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
          break;
        }
        else
        {
            //checck for hidden items
            if(entry->d_name[0]=='.')
              continue;
            dirContentArr[i]=malloc(sizeof(char*)*strlen(entry->d_name)+1);
            strcpy(dirContentArr[i++],entry->d_name);
            if(maxLen<strlen(entry->d_name)) maxLen = strlen(entry->d_name);
        }
    }
    for(int i = 0; i<dirCount;i++)
    {

      if(i%3==0 && i>0)
      {
        printf("\n");
      }
      printf("%-15s",dirContentArr[i]);
      free(dirContentArr[i]);
    }
    closedir(dp);
    printf("\n");
}
int getDirContentCount(char* dir)
{
  DIR* dp = opendir(dir);
  errno = 0;
  int conCounter = 0;
  struct dirent* entry;
  while(1)
  {
    entry = readdir(dp);
    if(entry!=NULL && errno!=0)
    {
        perror("readdir failed");
        exit(errno);
    }
    if(entry==NULL && errno==0)
    {
        break;
    }
    else
    {
      if(entry->d_name[0]=='.')
        continue;
      conCounter++;
    }
  }
  closedir(dp);
  return conCounter;
}
