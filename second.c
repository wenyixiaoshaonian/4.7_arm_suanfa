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

int major;

static int second_dev_open(struct inode *inode, struct file *file)
{
  return 0;
}

static ssize_t second_dev_write(struct file *file, const char __user *buf, size_t count, loff_t * ppos)
{
  return 0;
}

static int second_dev_read(struct file *filp, char __user *buff, size_t count, loff_t *offp)
{
  return 0;
}

static int second_drv_init(void)
{
  register_chrdev(0,"second_drv", &second_dev_fops);
  
  seconddrv_class = class_create(THIS_MODULE, "second_drv");

  seconddrv_class_dev = class_device_create(seconddrv_class, NULL, MKDEV(major, 0), NULL, "buttons");
  return 0;
}

static int second_drv_exit(void)
{
  major = unregister_chrdev(major,"second_drv");
  class_device_unregister(seconddrv_class_dev);
=  class_destroy(seconddrv_class);
  return 0;
}


static struct file_operations second_dev_fops = {
		.owner	=	THIS_MODULE,	
		.open	=	second_dev_open,	   	   
		.write	=	second_dev_write,    
		.read	=	second_dev_read,    
	};


module_init(second_drv_init);
module_exit(second_drv_exit);

MODULE_LICENSE("GPL");

