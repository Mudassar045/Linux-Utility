#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/types.h>


#define MAX_LEN 512
#define MAXARGS 10
#define ARGLEN 30
#define PROMPT "mudassar@Seraskier:~ "
char* read_cmd(char* prompt,FILE *fp);
char** tokenize(char*);
int execute(char* arglist[]);
int IsInternal(char* cmd);
void printHelpInfo(char* cmd);
void progExit();
int Num_Args = 0;
int BG_FLAG = 0;
int main()
{

  char *cmdline;
  char** arglist;
  char arr[8][10];
  int j;
  char* prompt=PROMPT;
  while((cmdline = read_cmd(prompt,stdin)) != NULL)
  {

    char *multicmd = strtok(cmdline,";");
    for(;multicmd!=NULL;)
    {
      if(*multicmd=='&')
      {
        system(multicmd);
      }
      else if((arglist = tokenize(multicmd)) != NULL)
      {
        execute(arglist);
      }
      multicmd = strtok(NULL,";");
    }
    for(j=0;j<MAXARGS+1;j++)
    free(arglist[j]);
    free(arglist);
    free(cmdline);
  }
  printf("\n");
  return 0;

}
char* read_cmd(char* prompt,FILE* fp)
{
  printf("%s",prompt);
  char* cmdline = (char*)malloc(sizeof(char)*MAX_LEN);
  int c=0;
  int pos = 0;
  while((c=getc(fp)) !=EOF)
  {

    if(c== '\n')
    break;
    cmdline[pos++] =c;
  }
  if(c==EOF && pos==0)
  return NULL;
  cmdline[pos] = '\0';
  return cmdline;
}
char **tokenize(char* cmdline)
{
  char **arglist = (char**)malloc(sizeof(char*)*MAXARGS+1);
  int i;
  for(i=0;i<MAXARGS+1;i++)
  {
    arglist[i] = (char*)malloc(sizeof(char)*ARGLEN);
    bzero(arglist[i],ARGLEN);
  }

  if(cmdline[0]=='\0')
  return NULL;
  char* cp = cmdline;
  char * start;
  int len;
  int argnum = 0;

  while(*cp != '\0')
  {
    while(*cp ==' ' || *cp == '\t')
    {
      cp++;
    }

    start = cp;
    len = 1;
    while(*++cp != '\0' && !(*cp == ' ' || *cp == '\t'))
    len++;
    strncpy(arglist[argnum],start,len);
    arglist[argnum][len] = '\0';

    argnum++;
  }
  arglist[argnum] = NULL;
  Num_Args = argnum;
  return arglist;
}
int execute(char* arglist[])
{
  int status;
  status = IsInternal(arglist[0]);
  if(status!=0)
  {

    for(int i=1;i<=Num_Args;i++)
    {
      if((strcmp(arglist[i],"&")==0) || (strcmp(arglist[i],"bg")==0))
      {
        BG_FLAG = 1;
        arglist[i]=NULL;
        break;
      }

    }
    
    int cpid = fork();
    switch(cpid)
    {
      case -1:
      perror("fork failed");
      exit(1);
      case 0:
      {
        execvp(arglist[0],arglist);
        perror("exec failed");
        exit(1);
      }
      default:
      {
        if(BG_FLAG==1)
        {
          BG_FLAG = 0;
          return 0;
        }
        else
        {
          waitpid(cpid,&status,0);
          printf("child exitted with status %d\n",status>>8);
          return 0;
        }
      }
    }
  }
  else
  {
    if(strcmp(arglist[0],"help")==0) printHelpInfo(arglist[1]);
    if(strcmp(arglist[0],"exit")==0) progExit();
    if(strcmp(arglist[0],"cd")==0);
  }
}

int IsInternal(char * cmd)
{
  if(strcmp(cmd,"exit")==0) return 0;
  else if(strcmp(cmd,"cd")==0) return 0;
  else if(strcmp(cmd,"exit")==0) return 0;
  else if(strcmp(cmd,"help")==0) return 0;
  else return -1;
}
void printHelpInfo(char *cmd)
{
  if(strcmp(cmd,"cd")==0)
  {
    printf("cd: cd [-L|[-P [-e]] [-@]] [dir]\n\tChange the shell working directory.\n");
    printf("\tChange the current directory to DIR.  The default DIR is the value of the\n\tHOME shell variable.\n\n");
    printf("\tThe variable CDPATH defines the search path for the directory containing DIR.\n\tAlternative directory names in CDPATH are separated by a colon (:).\n\tA null directory name is the same as the current directory.  If DIR begins with a slash (/), then CDPATH is not used.\n\n");
    printf("\tIf the directory is not found, and the shell option `cdable_vars' is set,\n\tthe word is assumed to be  a variable name.\n\tIf that variable has a value, its value is used for DIR.\n\n");
  }
  if(strcmp(cmd,"pwd")==0)
  {
    printf("pwd: pwd [-LP]\n");
    printf("\tPrint the name of the current working directory.\n");
    printf("\tOptions: \n");
    printf("\t-L	 print the value of $PWD if it names the current working directory\n");
    printf("\t-P   -P	print the physical directory, without any symbolic links\n");
  }
}
void progExit(){exit(0);}
