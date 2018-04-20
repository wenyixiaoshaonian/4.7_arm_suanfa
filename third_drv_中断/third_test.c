#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>

unsigned char key_val;


int main(int argc,char **argv)
{

  int fd;
  int cnt = 0;

  
  fd = open("/dev/buttons",O_RDWR);
  if (fd < 0)
    {
        printf("error, can't open \n");
    }
  while(1)
  	{
  	  read(fd, &key_val, 1);
	  printf("key_val = 0x%x \n",key_val);
  	}
  return 0;

}




