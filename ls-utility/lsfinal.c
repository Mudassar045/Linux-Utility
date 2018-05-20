#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>
#include <sys/types.h>
#include <errno.h>
#include <pwd.h>
#include <grp.h>

/* COLOR CONSTATNS*/

#define KRED   "\033[1;31m" /*tar: for tar balll and misc files*/
#define KYEL   "\033[33m"   /* pi: Pipe: yellow/brown */
#define KLYEL  "\033[1;33m" /* cd: Char device and Block device:  bright yellow */
#define KLBLU  "\033[1;34m" /* di: Directory: bright blue */
#define KLMGN  "\033[1;35m" /* so: Socket: bright magenta */
#define KLCYN  "\033[1;36m" /* ln: Symlink: bright cyan */
#define KWHT   "\033[0m"    /* bright white*/
#define KRESET "\033[0m"    /* No color: Resetter*/
#define KLGRN  "\033[1;32m" /* regular files bright green*/

/*global contants*/
extern int errno;
int DIR_COUNT  = 0; /*const: counter for directory*/
int IS_ONE_SET = 0; /*const: checking -1 arg*/
int IS_LS_SET  = 0; /*const: long listing*/
int IS_R_SET   = 0; /*const: recursive simple*/
int IS_RS_SET  = 0; /*const: recurisve long listing*/

void lsOnSubDIR(char**,int,int,char*); /*do_ls for sub directories*/
void showDirContent(char*);

void showSubDirContent(char*,char*);
void frmtprintf(char*,struct stat); /*formatted output using built in printf*/
void memDeallocator(char**,int);
void getPermissionChars(char*,int);
void getUsernameByUid(char*, int);
void getGroupNameByGid(char*, int);

int hasSubDirectory(char*);
int getTotalBlocks(char**,int,char*);
int compareStrings(const void*, const void*);
int getDirContentCount(char*);
int hasSubDirectory(char*);
int isOption(char*);
char **do_ls(char*);
char getFileType(int);
char **getSortedDirContent(char*,int);

/*printing functions with args*/

void frmtPrintf_1(char*,struct stat);
void frmtPrintf_2(char*,struct stat);
void showDirContent(char*);
void showSubDirContents(char*);
int main(int argc, char**argv)
{
    // reading content of pwd/cwd directory
    if(argc==1)
    {
      IS_ONE_SET = 0;
      showDirContent(".");
    }
    if(argc==2 && isOption(argv[1]))
    {
      if(strcmp(argv[1],"-1")==0)
      {
        IS_ONE_SET = 1;
        showDirContent(".");
      }
      else if(strcmp(argv[1],"-R" )==0)
      {
        IS_R_SET = 1;
        showDirContent(".");
      }
    }
    exit(0);
}
int isOption(char* opt)
{
  if(opt[0]=='-') return 1;
  else return 0;
}
void showSubDirContent(char*subdir,char* dir)
{
  char** dirContentArr = NULL;
  int dirCount = 0;
  struct stat statFile;
  int rv = 0;
  dirCount = getDirContentCount(subdir);
  if(dirCount>0)
  {
      // getting sorted dirContents
      // getting sorted dirContents

      dirContentArr = getSortedDirContent(subdir,dirCount);
      for(int i = 0; i<dirCount;i++)
      {
        if(dir!=NULL)
        {
          char buff[32];
          strcpy(buff,dir);
          strcat(buff,"/");
          strcat(buff,subdir);
          strcat(buff,"/");
          strcat(buff,dirContentArr[i]);
          rv = stat(buff, &statFile);
          //printf("%s\n",buff);
        }
        if (rv)
        {
          perror ("stat");
          exit(rv);
        }
        if(IS_ONE_SET) frmtPrintf_2(dirContentArr[i],statFile);
        else frmtPrintf_1(dirContentArr[i],statFile);
     }
     if(!(IS_ONE_SET))
     printf("\n"); /* new line at the end */
     memDeallocator(dirContentArr, dirCount); /*memory deallcator*/
   }
}
void showDirContent(char* dir)
{
  int dirCount = 0;
  char **dirContentArr = NULL;
  struct stat statFile;
  int sub_dir_count = 0;
  dirCount = getDirContentCount(dir);
  dirContentArr = getSortedDirContent(dir,dirCount);

  for(int i = 0; i<dirCount;i++)
  {
      int rv = stat(dirContentArr[i], &statFile);
      if (rv)
      {
        perror ("stat");
        exit(rv);
      }
      if(IS_ONE_SET) frmtPrintf_2(dirContentArr[i],statFile);
      else if(IS_R_SET)
      {
        if(hasSubDirectory(dirContentArr[i]))
          sub_dir_count++;
        frmtPrintf_1(dirContentArr[i],statFile);
      }
      else frmtPrintf_1(dirContentArr[i],statFile);

   }
   if(!(IS_ONE_SET))
   printf("\n"); /* new line at the end */
   if(!(IS_R_SET))
   memDeallocator(dirContentArr, dirCount); /*memory deallcator*/
   else
   {
     if(sub_dir_count>0)
     lsOnSubDIR(dirContentArr, sub_dir_count, dirCount,dir);
   }
}
void lsOnSubDIR(char **dirContentArr,int sub_dir_count, int dirCount,char* dir)
{
    char ** Sub_Dir_Arr = NULL;
    struct stat statFile;
    int d = 0;
    Sub_Dir_Arr = malloc(sizeof(char*)*sub_dir_count);
    for(int i = 0; i<dirCount; i++)
    {
        if(hasSubDirectory(dirContentArr[i]))
        {
            Sub_Dir_Arr[d]=malloc(sizeof(char*)*(strlen(dirContentArr[i])+1));
            strcpy(Sub_Dir_Arr[d++],dirContentArr[i]);
        }
    }

    memDeallocator(dirContentArr,dirCount); /*deallocating memory*/

    for(int i =0; i<sub_dir_count;i++)
    {
      printf("\n./%s: \n", Sub_Dir_Arr[i]);
      showSubDirContent(Sub_Dir_Arr[i],dir);
    }
}
void frmtPrintf_1(char *d_name, struct stat statFile)
{
  char ftype = getFileType(statFile.st_mode);
  if(ftype!='d'&& (statFile.st_mode & S_IXUSR||statFile.st_mode & S_IXGRP||statFile.st_mode & S_IXOTH))
  {
      printf("%s%s%s  ",KRESET,KLGRN, d_name);
  }
  else
  {
    switch (ftype)
    {
      case '-':printf("%s%s%s  ",KRESET,KWHT, d_name);break;
      case 'd':printf("%s%s%s  ",KRESET,KLBLU, d_name);break;
      case 'c':printf("%s%s%s  ",KRESET,KLYEL, d_name);break;
      case 'b':printf("%s%s%s  ",KRESET,KLYEL, d_name);break;
      case 'l':printf("%s%s%s  ",KRESET,KLCYN, d_name);break;
      case 's':printf("%s%s%s  ",KRESET,KLMGN, d_name);break;
      case 'p':printf("%s%s%s  ",KRESET,KLYEL, d_name);break;
    }
  }
}
void frmtPrintf_2(char *d_name, struct stat statFile)
{
  char ftype = getFileType(statFile.st_mode);
  if(ftype!='d'&& (statFile.st_mode & S_IXUSR||statFile.st_mode & S_IXGRP||statFile.st_mode & S_IXOTH))
  {
      printf("%s%s%s\n",KRESET,KLGRN, d_name);
  }
  else
  {
    switch (ftype)
    {
      case '-':printf("%s%s%s\n",KRESET,KWHT, d_name);break;
      case 'd':printf("%s%s%s\n",KRESET,KLBLU, d_name);break;
      case 'c':printf("%s%s%s\n",KRESET,KLYEL, d_name);break;
      case 'b':printf("%s%s%s\n",KRESET,KLYEL, d_name);break;
      case 'l':printf("%s%s%s\n",KRESET,KLCYN, d_name);break;
      case 's':printf("%s%s%s\n",KRESET,KLMGN, d_name);break;
      case 'p':printf("%s%s%s\n",KRESET,KLYEL, d_name);break;
    }
  }
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
char getFileType(int st_mode)
{
   if(S_ISDIR(st_mode)) return 'd';
   else if(S_ISREG(st_mode)) return '-';
   else if(S_ISLNK(st_mode)) return 'l';
   else if(S_ISCHR(st_mode)) return 'c';
   else if(S_ISBLK(st_mode)) return 'b';
   else if(S_ISSOCK(st_mode))return 's';
   else if(S_ISFIFO(st_mode))return 'p';
   else return '?';
}
char **getSortedDirContent(char*dir, int dirCount)
{
  DIR* dp = opendir(dir);
  errno = 0;
  struct dirent* entry;
  int k = 0;
  char** dirContentArr = malloc(sizeof(char*)* (dirCount));
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
        //Sorting records
        qsort (dirContentArr, dirCount, sizeof ( *dirContentArr),
           compareStrings );
          return dirContentArr;
      }
      else
      {
          if(entry->d_name[0]=='.')
            continue;
          dirContentArr[k] = (char*)malloc(sizeof(char)*(strlen(entry->d_name)+1));
          strcpy(dirContentArr[k++],entry->d_name);
      }
  }
  //closing directory
  closedir(dp);
}
int compareStrings(const void *sptr1, const void *sptr2)
{
  const char *a=*(const char**)sptr1;
  const char *b=*(const char**)sptr2;
  return strcmp(a,b);
}
void memDeallocator(char** dirContentArr, int dirCount)
{
   for(int i = 0; i<dirCount;i++) free(dirContentArr[i]);/*deallocating memory*/
}
int hasSubDirectory(char* dir)
{
     DIR* dp = opendir(dir);
     if(dp==NULL)
     {
        closedir(dp);
        return 0;
     }
     else
     {
        closedir(dp);
        return 1;
     }

}
int hasSubDirectoryHasContent(char* dir)
{
    DIR* dp = opendir(dir);
    errno = 0;
    struct dirent* entry;
    entry = readdir(dp);
    if(entry==NULL && entry==0)
    {
        closedir(dp);
        return 0;
    }
    else
    {
        closedir(dp);
        return 1;
    }
}
