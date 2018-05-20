#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<sys/wait.h>
#include<sys/types.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/stat.h>
#include <fcntl.h>

#define MAX_HIST_SIZE 512
#define MAX_HIST_LEN 32
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
void showHistory(char* hist[],int histcount);
int progChangeCWD(char *dir);
void progGetEnv();
int progSetEnv(char* envVar, char* Val);
int progUpdateEnv(char *envVar,char* nVal);
int main()
{

  char *cmdline;
  char** arglist;
  char **cmdHist = (char**)malloc(sizeof(char*)*(MAX_HIST_SIZE+1));
  int j;
  int m=0;
  char* prompt=PROMPT;
  for(m=0; m<MAX_HIST_SIZE;m++)
  {
    cmdHist[m] = (char*)malloc(sizeof(char)*MAX_HIST_LEN);
  }
  m = 0;
  while((cmdline = read_cmd(prompt,stdin)) != NULL)
  {
    char *multicmd = strtok(cmdline,";");
    for(;multicmd!=NULL;)
    {
      strcpy(cmdHist[m],multicmd);
      m++;
      if(strcmp(multicmd,"!n")==0)
      {
        showHistory(cmdHist,m);
      }
      else if(*multicmd=='&')
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
  //char* cmdline = (char*)malloc(sizeof(char)*MAX_LEN);
  char* cmdline, shell_prompt[100];
  // Configure readline to auto-complete paths when the tab key is hit.
  rl_bind_key('\t', rl_complete);
  for(;;) {
      // Create prompt string from user name and current working directory.
        snprintf(shell_prompt, sizeof(shell_prompt), "%s:%s $ ", getenv("USER"), getcwd(NULL, 1024));
      // Display prompt and read input (NB: input must be freed after use)...
      cmdline = readline(prompt);
      // Check for EOF.
      if (!cmdline)
          break;
      // Add input to history.
      add_history(cmdline);
      return cmdline;
    }
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
  return arglist;
}
int execute(char* arglist[])
{
  int status;
  status = IsInternal(arglist[0]);
  if(status!=0)
  {
    int cpid = fork();
    switch(cpid)
    {

      case -1:
      perror("fork failed");
      exit(1);
      case 0:
      execvp(arglist[0],arglist);
      perror("exec failed");
      exit(1);
      default:
      waitpid(cpid,&status,0);
      printf("child exitted with status %d\n",status>>8);
      return 0;
    }
  }
  else
  {
    if(strcmp(arglist[0],"help")==0) printHelpInfo(arglist[1]);
    if(strcmp(arglist[0],"exit")==0) progExit();
    if(strcmp(arglist[0],"cd")==0)
    {
      int rev = 0;
      rev = progChangeCWD(arglist[1]);
      if(rev==-1)
      {
        fprintf(stderr, "Unable to change directory\n");
      }
    }
    if(strcmp(arglist[0],"get-env")==0)
    {
      progGetEnv();
    }
    if(strcmp(arglist[0],"set-env")==0)
    {
      progSetEnv(arglist[1],arglist[2]);
    }
    if(strcmp(arglist[0],"upd-env")==0)
    {

    }
  }
}
int IsInternal(char * cmd)
{
  if(strcmp(cmd,"exit")==0) return 0;
  else if(strcmp(cmd,"cd")==0) return 0;
  else if(strcmp(cmd,"exit")==0) return 0;
  else if(strcmp(cmd,"help")==0) return 0;
  else if(strcmp(cmd,"set-env")==0) return 0;
  else if(strcmp(cmd,"get-env")==0) return 0;
  else if(strcmp(cmd,"upd-env")==0) return 0;
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
void progExit()
{
  exit(0);
}
void showHistory(char* hist[],int histcount)
{
  printf("\n");
  if(histcount>0)
  for(int i = 0; i<histcount && hist[i]!=NULL;i++)
  {
    printf("%d  %s\n",i+1, hist[i]);
  }
  else printf("!n: there is no history\n");
}
int progChangeCWD(char *dirname)
{
  int rev = chdir(dirname);
  return rev;
}
void progGetEnv()
{
  FILE *fd = fopen("mdshellenv.txt","r");
  if(fd==NULL)
  {
    fprintf(stderr, "Unable to create or open a file\n");
  }
  char string[100];
  while(fgets(string, 100, fd))
  {
    printf("%s", string);
  }
  fclose(fd);
}
int progSetEnv(char* envVar, char* Val)
{
  FILE *fd = fopen("mdshellenv.txt","a+");
  if(fd==NULL)
  {
    fprintf(stderr, "Unable to create or open a file\n");
  }
  int results = fputs(envVar, fd);
  results = fputs(Val, fd);
  results = fputc('\n',fd);
  if (results == EOF)
  {
   fprintf(stderr, "Unable to set environment variable\n");
  }
  fclose(fd);
  return results;
}
int progUpdateEnv(char* envVar, char* nVal)
{

}
