#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>

#define MINORBITS	20
#define MINORMASK	((1U << MINORBITS) - 1)

#define MINOR(dev)	((unsigned int) ((dev) & MINORMASK))


static struct class *firstdrv_class;
static struct class_device *firstdrv_class_dev[4];

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

static int first_dev_open(struct inode *inode, struct file *file)
{

  *gpfcon &= ~((0x3 << (4*2)) | (0x3 << (5*2)) | (0x3 << (6*2)));
  *gpfcon |=  ((0x1 << (4*2))  | (0x1 << (5*2))  | (0x1 << (6*2)));
  return 0;
}
int major;
static ssize_t first_dev_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
	{
	  int minor = MINOR(file->f_dentry->d_inode->i_rdev);
      int val;
	  copy_from_user(&val,buf,count);
   switch(minor)
   	{
   	  case 0 :
   	  	{
	  if (val == 1)
	  	*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	  else 
	  	*gpfdat &= ~((1<<4) | (1<<5) | (1<<6));
   	  	}
	  case 1 :
	  	{
	  if (val == 1)
	  	*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	  else 
	  	*gpfdat &= ~(1<<4) ;
	  break;
	  	}
	  case 2 :
	  	{
	  if (val == 1)
	  	*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	  else 
	  	*gpfdat &= ~(1<<5) ;
	  break;
	  	}
	  case 3 :
	  	{
	  if (val == 1)
	  	*gpfdat |= (1<<4) | (1<<5) | (1<<6);
	  else 
	  	*gpfdat &= ~(1<<6) ;
	  break;
	  	}
	  default :
	    break;
   	}  
	  return 0;
	}


	static struct file_operations first_dev__fops = {
		.owner	=	THIS_MODULE,	
		.open	=	first_dev_open,	   	   
		.write	=	first_dev_write,    
	};


int firse_drv_init(void)
{
  int minor = 0; 

  major = register_chrdev(0, "first_drv", &first_dev__fops);

  firstdrv_class = class_create(THIS_MODULE, "leds");

  firstdrv_class_dev[0] = class_device_create(firstdrv_class, NULL, MKDEV(major, 0), NULL, "leds");

 for (minor = 1; minor < 4; minor++)
  	{
  	  firstdrv_class_dev[minor] = class_device_create(firstdrv_class, NULL, MKDEV(major, minor), NULL, "led%d", minor);
  	}

  gpfcon = (volatile unsigned long*)ioremap(0x56000050,16);

  gpfdat = gpfcon+1;
  
  return 0;
}

void firse_drv_exit(void)
{
  int minor;
  major = unregister_chrdev(0, "leds");

  class_device_unregister(firstdrv_class_dev[0]);
  class_destroy(firstdrv_class);
  for (minor = 0; minor < 4; minor++)
	{
		class_device_unregister(firstdrv_class_dev[minor]);
	}
  iounmap(gpfcon);
}

module_init(firse_drv_init);
module_exit(firse_drv_exit);

MODULE_LICENSE("GPL");
