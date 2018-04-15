
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

unsigned char key_vals[4];


int main(int argc,char **argv)
{

  int fd;
  int cnt = 0;

  
  fd = open("/dev/buttons",O_RDWR);
  if (fd < 0)
    {
        printf("error, can't open \n");
        return 0;
    }
  while(1)
  	{
  	  read(fd,key_vals,sizeof(key_vals));
	  if(!key_vals[0]||!key_vals[1]||!key_vals[2]||!key_vals[3])
	  	{
	  	  printf("%04d key pressed:%d %d %d %d",cnt++,key_vals[0],key_vals[1],key_vals[2],key_vals[3]);
	  	}
  	}
  return 0;

}




