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
extern int errno;
void do_ls(char*);
void frmtprintf(char*,struct stat);
void getPermissionChars(char*,int);
void getUsernameByUid(char*, int);
void getGroupNameByGid(char*, int);
int countDirectoryContents(char*);
char **sortedDirectoryContents(DIR*,int);
char getFileType(int);
int compareStrings(const void *stackA, const void *stackB);

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
static int cmpstringp(const void *p1, const void *p2)
{
   /* The actual arguments to this function are "pointers to
      pointers to char", but strcmp(3) arguments are "pointers
      to char", hence the following cast plus dereference */
      return strcmp(* (char * const *) p1, * (char * const *) p2);
}

void do_ls(char* dir)
{
    char** dirContentArr = NULL;
    int dirContent = 0;
    DIR* dp = opendir(dir);
    //display file or file system status
    struct stat statFile;
    struct dirent* entry;
    errno = 0;
    dirContent = countDirectoryContents(dir);
    if(dp==NULL)
    {
        fprintf(stderr, "Can't open directory: %s",dir);
        exit(errno);
    }
    else
    {
            dirContentArr = sortedDirectoryContents(dp,dirContent);
            //printf("%ld\n",sizeof(dirContentArr));
            for(int i = 0; i<dirContent;i++)
            {
                int rv = stat(dirContentArr[i], &statFile);
                if (rv)
                {
                  perror ("stat");
                  exit(rv);
                }
                // my printf
                frmtprintf(dirContentArr[i], statFile);
            }
    }
    // deallocating memory
    for (int i = 0; i<dirContent;i++)
    {
      free(dirContentArr[i]);
    }
    // clsing directory
    closedir(dp);
}
char **sortedDirectoryContents(DIR* dp, int dirContent)
{
  struct dirent* entry;
  errno = 0;
  int k = 0;
  char** dirContentArr = malloc(sizeof(char*)* (dirContent));
  while(k<dirContent)
  {
      entry=readdir(dp);
      if(entry!=NULL && errno!=0)
      {
          perror("readdir failed");
          exit(errno);
      }
      else
      {
          if(entry->d_name[0]=='.')
            continue;
          dirContentArr[k] = (char*)malloc(sizeof(char)*(strlen(entry->d_name)+1));
          strcpy(dirContentArr[k++],entry->d_name);
      }
  }
  //Sorting records
  qsort (dirContentArr, dirContent, sizeof ( *dirContentArr),
     compareStrings );
  //return directory content array
  return dirContentArr;
}
int compareStrings(const void *stackA, const void *stackB)
{
  const char *a=*(const char**)stackA;
  const char *b=*(const char**)stackB;
  return strcmp(a,b);
}
int isDirectory(char* dir)
{
     DIR* dp = opendir(dir);
     if(dp==NULL)
     {
         return 0;
     }
     else
     {
         return 1;
     }
}
int isDirectoryHasContent(char* dir)
{
    DIR* dp = opendir(dir);
    errno = 0;
    struct dirent* entry;
    entry = readdir(dp);
    if(entry==NULL && entry==0)
    {
        return 0;
    }
    else
    {
        return 1;
    }
}

int countDirectoryContents(char *dir)
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
  // file info getters
  getPermissionChars(permChars,statFile.st_mode);
  getUsernameByUid(userName,statFile.st_uid);
  getGroupNameByGid(groupName,statFile.st_gid);
  // get date format like ls
  strftime(fileModificationTime, sizeof(fileModificationTime), "%b  %d %H:%M", localtime(&(statFile.st_ctime)));
  //printing long listing like ls
  printf("%2ld %s %1ld %s %s %5ld %s %s\n",statFile.st_blocks,permChars,
  statFile.st_nlink,userName,groupName,statFile.st_size,fileModificationTime,d_name);
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
