
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

int main(int argc,char **argv)
{

  int fd;
  int val = 1;
  char* filename;
  
  filename = argv[1];
  
  fd = open(filename,O_RDWR);
  if (fd < 0)
    {
        printf("error, can't open %s\n", filename);
        return 0;
    }
  if (argc !=3)
  	{
  	  printf("Usage : \n");
	  printf("%s %d <on|off>\n",argv[0],argv[1]);
	  return 0;
  	}
  if (strcmp(argv[2],"on") == 0)
  	val = 0;
  else 
  	val = 1;
  
  write(fd,&val,4);
  return 0;
}

