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

#define KRED   "\x1B[31m"
#define KGRN   "\x1B[32m"
#define KYEL   "\x1B[33m"
#define KLYEL  "\x1B[1;33m"
#define KLBLU  "\033[1;34m"
#define KLMGN  "\033[1;35m"
#define KLCYN  "\x1B[1;36m"
#define KWHT   "\x1B[0m"
#define KRESET "\x1B[0m"
#define KLGRN  "\033[1;32m"

extern int errno;
void frmtprintf(char*,struct stat);
char **do_ls(char*);
void lsOnSubDIR(char*);
void getPermissionChars(char*,int);
void getUsernameByUid(char*, int);
void getGroupNameByGid(char*, int);
int  countOfDirectoryContent(char*);
int hasSubDirectory(char*);
void showDirContents(char *);
void showSubDirContents(char*);
char **sortedDirectoryContents(char*,int);
int getTotalBlocks(char **,int,char*);
char getFileType(int);
int compareStrings(const void *stackA, const void *stackB);
int DIR_COUNT=0;
// Main
int main(int argc, char**argv)
{
    // reading content of pwd/cwd directory
    if(argc==1)
    {
        showDirContents(".");
    }
    else
    {
        // printing content of given directory
        int i = 0;
        while(++i<argc)
        {
            printf("Directory listing of %s:\n",argv[i]);
            showDirContents(argv[i]);
        }
    }
    exit(0);
}
void showDirContents(char *dir)
{
  char ** Dir_Arr = NULL;
  char ** Sub_Dir_Arr = NULL;
  Dir_Arr = do_ls(dir);
  int sub_dir_count = 0;
  int d = 0;
  for(int i = 0; i<DIR_COUNT; i++)
  {
    if(hasSubDirectory(Dir_Arr[i]))
      sub_dir_count++;
  }
  Sub_Dir_Arr = malloc(sizeof(char*)*sub_dir_count);
  for(int i = 0; i<DIR_COUNT; i++)
  {

      if(hasSubDirectory(Dir_Arr[i]))
      {
          Sub_Dir_Arr[d]=malloc(sizeof(char*)*(strlen(Dir_Arr[i])+1));
          strcpy(Sub_Dir_Arr[d++],Dir_Arr[i]);
      }

  }
  for(int i = 0; i<DIR_COUNT; i++)
  {
    free(Dir_Arr[i]);
  }
  for(int i =0; i<sub_dir_count;i++)
  {
    printf("\n./%s: \n", Sub_Dir_Arr[i]);
    lsOnSubDIR(Sub_Dir_Arr[i]);
  }
}
void lsOnSubDIR(char *dir)
{
  char** dirContentArr = NULL;
  //display file or file system status
  int dirCount = 0;
  struct stat statFile;
  dirCount = countOfDirectoryContent(dir);
  if(dirCount>0)
  {
      // getting sorted dirContents
      // getting sorted dirContents
      char buff[32];
      dirContentArr = sortedDirectoryContents(dir,dirCount);
      printf("total %d\n", (getTotalBlocks(dirContentArr,dirCount,dir))/2);
      for(int i = 0; i<dirCount;i++)
      {
        if(dir!=".")
        {
          strcpy(buff,dir);
          int len = strlen(buff);
          buff[len] = '/';
          buff[len+1]='\0';
          strcat(buff,dirContentArr[i]);
          int rv = stat(buff, &statFile);
          if (rv)
          {
            perror ("stat");
            exit(rv);
          }
        }
        else
        {
          int rv = stat(dirContentArr[i], &statFile);
          if (rv)
          {
            perror ("stat");
            exit(rv);
          }
         }
          frmtprintf(dirContentArr[i], statFile);
      }
  }
  // deallocating memory
  for (int i = 0; i<dirCount;i++)
  {
    free(dirContentArr[i]);
  }
}
char** do_ls(char* dir)
{
    char** dirContentArr = NULL;
    //display file or file system status
    int dirCount = 0;
    struct stat statFile;
    dirCount = countOfDirectoryContent(dir);
    //printf("%d\n", dirCount);
    DIR_COUNT = dirCount;
    if(dirCount>0)
    {
            // getting sorted dirContents
            char buff[32];
            dirContentArr = sortedDirectoryContents(dir,dirCount);
            printf("total %d\n", (getTotalBlocks(dirContentArr,dirCount,dir))/2);
            for(int i = 0; i<dirCount;i++)
            {
              if(dir!=".")
              {
                strcpy(buff,dir);
                int len = strlen(buff);
                buff[len] = '/';
                buff[len+1]='\0';
                strcat(buff,dirContentArr[i]);
                int rv = stat(buff, &statFile);
                if (rv)
                {
                  perror ("stat");
                  exit(rv);
                }
              }
              else
              {
                int rv = stat(dirContentArr[i], &statFile);
                if (rv)
                {
                  perror ("stat");
                  exit(rv);
                }
               }

                frmtprintf(dirContentArr[i], statFile);
            }
    }
    return dirContentArr;
}
int getTotalBlocks(char **dirContentArr,int dirCount, char *dir)
{
  struct stat statFile;
  int noBlocks = 0;
  char buff[20];
  for(int i = 0; i<dirCount;i++)
  {
    if(dir!=".")
    {
      strcpy(buff,dir);
      int len = strlen(buff);
      buff[len] = '/';
      buff[len+1]='\0';
      strcat(buff,dirContentArr[i]);
      int rv = stat(buff, &statFile);
      if (rv)
      {
        perror ("stat");
        exit(rv);
      }
    }
    else
    {
      int rv = stat(dirContentArr[i], &statFile);
      if (rv)
      {
        perror ("stat");
        exit(rv);
      }
     }
     noBlocks+=statFile.st_blocks;
  }
  return noBlocks;
}
char **sortedDirectoryContents(char*dir, int dirCount)
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
int compareStrings(const void *stackA, const void *stackB)
{
  const char *a=*(const char**)stackA;
  const char *b=*(const char**)stackB;
  return strcmp(a,b);
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

int countOfDirectoryContent(char *dir)
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

void frmtprintf(char *d_name, struct stat statFile)
{
  // date format holder
  char fileModificationTime[20];

  //dynamic allocation
  char* userName  = (char*) malloc(sizeof(char)*32);
  char* groupName = (char*) malloc(sizeof(char)*32);
  char* permChars = (char*) malloc(sizeof(char)*12);
  //setting null value
  memset(userName,'\0',32);
  memset(groupName,'\0',32);
  memset(permChars,'\0',12);

  // file info getters
  getPermissionChars(permChars,statFile.st_mode);
  getUsernameByUid(userName,statFile.st_uid);
  getGroupNameByGid(groupName,statFile.st_gid);

  // get date format like ls
  strftime(fileModificationTime, sizeof(fileModificationTime), "%b  %d %H:%M", localtime(&(statFile.st_ctime)));
  //printing long listing like ls
  char ftype = getFileType(statFile.st_mode);

  // file size manip
  int INDET= 0;
  if(statFile.st_size<=9)
    {
      INDET = 1;
    }
  else if(statFile.st_size<=99)
    {
      INDET = 2;
    }
  else if(statFile.st_size<=999)
      {
        INDET = 3;
      }
  else if(statFile.st_size<=9999)
      {
        INDET = 4;
      }
  else if(statFile.st_size<=99999)
      {
        INDET = 5;
      }
  else if(statFile.st_size<=999999)
      {
        INDET = 6;
      }
  /* ex: Executable: bright green */
  if(ftype!='d'&& (statFile.st_mode & S_IXUSR||statFile.st_mode & S_IXGRP||statFile.st_mode & S_IXOTH))
  {
    printf("%s%2ld %s %1ld %s %s %5ld %s ",KWHT,statFile.st_blocks/2,permChars,
    statFile.st_nlink,userName,groupName,statFile.st_size,fileModificationTime);
    printf("%s%s%s\n",KRESET,KLGRN, d_name);
  }
  else if(ftype=='-')
  {
    printf("%s%2ld %s %1ld %s %s %5ld %s ",KWHT,statFile.st_blocks/2,permChars,
    statFile.st_nlink,userName,groupName,statFile.st_size,fileModificationTime);
    printf("%s%s%s\n",KRESET,KWHT, d_name);
  }
  /* di: Directory: bright blue */
  else if(ftype=='d')
  {
    printf("%s%2ld %s %1ld %s %s %5ld %s ",KWHT,statFile.st_blocks/2,permChars,
    statFile.st_nlink,userName,groupName,statFile.st_size,fileModificationTime);
    printf("%s%s%s\n",KRESET,KLBLU, d_name);
  }
  /* cd: Char device and Block device:  bright yellow */
  else if(ftype=='c' || ftype=='b')
  {
    printf("%s%2ld %s %1ld %s %s %5ld %s ",KWHT,statFile.st_blocks/2,permChars,
    statFile.st_nlink,userName,groupName,statFile.st_size,fileModificationTime);
    printf("%s%s%s\n",KRESET,KLYEL, d_name);
  }
  /* ln: Symlink: bright cyan */
  else if(ftype=='l')
  {
    printf("%s%2ld %s %1ld %s %s %5ld %s ",KWHT,statFile.st_blocks/2,permChars,
    statFile.st_nlink,userName,groupName,statFile.st_size,fileModificationTime);
    printf("%s%s%s\n",KRESET,KLCYN, d_name);
  }
  /* so: Socket: bright magenta */
  else if(ftype=='s')
  {
    printf("%s%2ld %s %1ld %s %s %5ld %s ",KWHT,statFile.st_blocks/2,permChars,
    statFile.st_nlink,userName,groupName,statFile.st_size,fileModificationTime);
    printf("%s%s%s\n",KRESET,KLMGN, d_name);
  }
  /* pi: Pipe: yellow/brown */
  else if(ftype=='p')
  {
    printf("%s%2ld %s %1ld %s %s %5ld %s ",KWHT,statFile.st_blocks/2,permChars,
    statFile.st_nlink,userName,groupName,statFile.st_size,fileModificationTime);
    printf("%s%s%s\n",KRESET,KYEL, d_name);
  }
  // deallocating memory
  free(userName);
  free(groupName);
  free(permChars);
}
void getPermissionChars(char*permChars, int st_mode)
{
  //checking file type
  permChars[0]=getFileType(st_mode);
  // user permissions
  permChars[1]=(char)(st_mode & S_IRUSR) ? '-' : 'r';
  permChars[2]=(char)(st_mode & S_IWUSR) ? 'w' : '-';
  permChars[3]=(char)(st_mode & S_IXUSR) ? 'x' : '-';
  // group permissions
  permChars[4]=(char)(st_mode & S_IRGRP) ? 'r' : '-';
  permChars[5]=(char)(st_mode & S_IWGRP) ? 'w' : '-';
  permChars[6]=(char)(st_mode & S_IXGRP) ? 'x' : '-';
  // others permissions
  permChars[7]=(char)(st_mode & S_IROTH) ? 'r' : '-';
  permChars[8]=(char)(st_mode & S_IWOTH) ? 'w' : '-';
  permChars[9]=(char)(st_mode & S_IXOTH) ? 'x' : '-';
}
void getUsernameByUid(char *userName, int st_uid)
{
  errno=0;
  struct passwd *pw = getpwuid(st_uid);
  if(pw!=NULL && pw!=0)
  {
    strncpy(userName,pw->pw_name,sizeof(pw->pw_name));
  }
  else
  {
    fprintf(stderr, "Unable to get username\n");
    exit(errno);
  }
}
void getGroupNameByGid(char* groupName, int st_gid)
{
  errno = 0;
  struct group  *gr = getgrgid(st_gid);
  if(gr!=NULL && gr!=0)
  {
    strncpy(groupName,gr->gr_name,sizeof(gr->gr_name));
  }
  else
  {
    fprintf(stderr, "Unable to get groupname\n");
    exit(errno);
  }
}
char getFileType(int st_mode)
{
   if(S_ISDIR(st_mode))
   {
     return 'd';
   }
   else if(S_ISREG(st_mode))
   {
     return '-';
   }
   else if(S_ISLNK(st_mode))
   {
     return 'l';
   }
   else if(S_ISCHR(st_mode))
   {
     return 'c';
   }
   else if(S_ISSOCK(st_mode))
   {
     return 's';
   }
   else if(S_ISFIFO(st_mode))
   {
     return 'p';
   }
   else if(S_ISBLK(st_mode))
   {
     return 'b';
   }
   else
   {
     return '?';
   }
}
