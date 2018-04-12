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

static struct class *firstdrv_class;
static struct class_device *firstdrv_class_dev;


static int first_dev_open(struct inode *inode, struct file *file)
{
  printk("first_dev_open\n");
  return 0;
}
int major;
static ssize_t first_dev_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
	{
	  printk("first_dev_write\n");
	  return 0;
	}


	static struct file_operations first_dev__fops = {
		.owner	=	THIS_MODULE,	
		.open	=	first_dev_open,	   	   
		.write	=	first_dev_write,    
	};


int firse_drv_init(void)
{
  major = register_chrdev(0, "first_drv", &first_dev__fops);

  firstdrv_class = class_create(THIS_MODULE, "firstdrv");

  firstdrv_class_dev = class_device_create(firstdrv_class, NULL, MKDEV(major, 0), NULL, "xyz");
  
  return 0;
}

void firse_drv_exit(void)
{
  major = unregister_chrdev(0, "first_drv");

  class_device_unregister(firstdrv_class_dev);
  class_destroy(firstdrv_class);
}

module_init(firse_drv_init);
module_exit(firse_drv_exit);

MODULE_LICENSE("GPL");
