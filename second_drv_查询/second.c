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

static struct class *seconddrv_class;
static struct class_device *seconddrv_class_dev;

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

volatile unsigned long *gpgcon = NULL;
volatile unsigned long *gpgdat = NULL;

int major;

static int second_dev_open(struct inode *inode, struct file *file)
{
  *gpfcon &= ~((0x3 << (0*2)) | (0x3 << (2*2)));
  
  *gpgcon &= ~((0x3 << (3*2)) | (0x3 << (11*2)));
  return 0;
}

static int second_dev_read(struct file *filp, char __user *buf, size_t size, loff_t *offp)
{
  unsigned char key_vals[4];
  int regval;

  if(size != sizeof(key_vals))
  	return -EINVAL;
  regval = *gpfdat;
  key_vals[0] = (regval &(1<<0)) ?1 : 0;
  key_vals[1] = (regval &(1<<1)) ?1 : 0;

  regval = *gpgdat;
  key_vals[2] = (regval &(1<<3)) ?1 : 0;
  key_vals[3] = (regval &(1<<11)) ?1 : 0;

  copy_to_user(buf,key_vals,sizeof(key_vals));
  return sizeof(key_vals);
}


	static ssize_t second_dev_write(struct file *file, const char __user *buf, size_t size, loff_t * ppos)
{
  return 0;
}

	static struct file_operations second_dev_fops = {
			.owner	=	THIS_MODULE,	
			.open	=	second_dev_open,		   
			.write	=	second_dev_write,	 
			.read	=	second_dev_read,	
		};
			

static int second_drv_init(void)
{
  major = register_chrdev(0,"second_drv", &second_dev_fops);
  
  seconddrv_class = class_create(THIS_MODULE, "second_drv");

  seconddrv_class_dev = class_device_create(seconddrv_class, NULL, MKDEV(major, 0), NULL, "buttons");   // /dev/buttons
  
  gpfcon = (volatile unsigned long*)ioremap(0x56000050,16);

  gpfdat = gpfcon+1;

  gpgcon = (volatile unsigned long*)ioremap(0x56000060,16);

  gpgdat = gpgcon+1;
  
  return 0;
}

static int second_drv_exit(void)
{
  major = unregister_chrdev(major,"second_drv");
  class_device_unregister(seconddrv_class_dev);
  class_destroy(seconddrv_class);

  iounmap(gpfcon);
  iounmap(gpgcon);
  return 0;
}



module_init(second_drv_init);
module_exit(second_drv_exit);

MODULE_LICENSE("GPL");

