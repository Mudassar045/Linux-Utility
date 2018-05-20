#include<stdio.h>
#include<stdlib.h>
#define PAGELEN 20
#define LINELEN 512
void do_more(FILE*);
int get_input(FILE*);
int main(int argc, char *argv[])
{
  if(argc==1)
  {
    do_more(stdin);
  }
  int i=0;
  FILE * fp;
  while(++i<argc)
  {
    fp= fopen(argv[i],"r");
    if (fp== NULL)
    {
      perror("Can't open file");
      exit(1);
    }
    do_more(fp);
    fclose(fp);
  }
}
void do_more(FILE *fp)
{
  int num_of_lines=0;
  int rv;
  char buffer[LINELEN];
  FILE* fp_tty = fopen("/dev//tty","r");
  while(fgets(buffer , LINELEN,fp))
  {
    fputs(buffer,stdout);
    num_of_lines++;
    if(num_of_lines == PAGELEN)
    {
      rv=get_input(fp_tty);
      if(rv == 0)
      {
        printf("\033[1A \033[2K \033[1G");
        break;
      }
      else if(rv==1)
      {
        printf("\033[1A \033[2K \033[1G");
        num_of_lines-=PAGELEN;
      }
      else if(rv==2)
      {
        printf("\033[1A \033[2K \033[1G");
        num_of_lines-=1;
      }
      else if(rv==3)
      {
        printf("\033[1A \033[2K \033[1G");
        break;
      }
    }
  }
}
int get_input(FILE* cmdstream)
{
  int c;
  printf("\033[7m --more--(%%) \033[m");
  c=getc(cmdstream);
  if(c=='q')
  return 0;
  if(c==' ')
  return 1;
  if(c=='\n')
  return 2;
  return 3;
  return 0;
}
int get_content_weight()
{

}
