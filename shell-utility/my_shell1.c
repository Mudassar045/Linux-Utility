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
int main()
{

  char *cmdline;
  char** arglist;
  int j;
  char* prompt=PROMPT;
  while((cmdline = read_cmd(prompt,stdin)) != NULL)
  {
    if((arglist = tokenize(cmdline)) != NULL)
    {
      execute(arglist);
      for(j=0;j<MAXARGS+1;j++)
      free(arglist[j]);
      free(arglist);
      free(cmdline);
    }
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
    cp++;

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
