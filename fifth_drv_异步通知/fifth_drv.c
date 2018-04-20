#include <linux/module.h>
#include <linux/poll.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/delay.h>
#include <asm/uaccess.h>
#include <linux/irq.h>
#include <asm/irq.h>
#include <asm/io.h>
#include <asm/arch/regs-gpio.h>
#include <asm/hardware.h>
#include <linux/poll.h>
static struct class *fifthdrv_class;
static struct class_device *fifthdrv_class_dev;

volatile unsigned long *gpfcon = NULL;
volatile unsigned long *gpfdat = NULL;

volatile unsigned long *gpgcon = NULL;
volatile unsigned long *gpgdat = NULL;


static unsigned char  key_val;

static DECLARE_WAIT_QUEUE_HEAD(button_waitq);

static volatile int ev_press = 0;  // 中断服务程序将它置一  fifth_drv_read 将它清零

static struct fasync_struct *button_async;

struct pin_desc{
	unsigned int pin;
	unsigned int key_val;   
};
// 键值按下时:0x01,0x02,0x03,0x04
// 键值松开是:0x81,0x81,0x83,0x84
struct pin_desc pins_desc[4] = 
{
    { S3C2410_GPF0,0X01},
	{ S3C2410_GPF2,0X02},
	{ S3C2410_GPG3,0X03},
	{ S3C2410_GPG11,0X04},


};
int major;
/*确定按键值*/
static irqreturn_t buttons_irq(int irq, void *dev_id)
{
    struct pin_desc * pindesc = (struct pin_desc *)dev_id;
	unsigned int pinval;
	
	pinval = s3c2410_gpio_getpin(pindesc->pin);
  //  printk("pinval:%d",pinval);
	if(pinval)
		{
       key_val =0x80 |pindesc-> key_val;
	}
	else 
		{
       key_val =   pindesc-> key_val;
	}

	
    ev_press = 1;                               // 中断发生
	wake_up_interruptible(&button_waitq);       // 唤醒休眠进程

	kill_fasync (&button_async, SIGIO, POLL_IN);
	
	return IRQ_RETVAL(IRQ_HANDLED);
}


static int fifth_dev_open(struct inode *inode, struct file *file)
{
 // *gpfcon &= ~((0x3 << (0*2)) | (0x3 << (2*2))); 
 // *gpgcon &= ~((0x3 << (3*2)) | (0x3 << (11*2)));输入引脚

 request_irq(IRQ_EINT0, buttons_irq,IRQT_BOTHEDGE,"s2",&pins_desc[0]);
 request_irq(IRQ_EINT2, buttons_irq,IRQT_BOTHEDGE,"s3",&pins_desc[1]);
 request_irq(IRQ_EINT11,buttons_irq,IRQT_BOTHEDGE,"s4",&pins_desc[2]);
 request_irq(IRQ_EINT19,buttons_irq,IRQT_BOTHEDGE,"s5",&pins_desc[3]);
  return 0;
}

static int fifth_dev_read(struct file *filp, char __user *buf, size_t size, loff_t *offp)
{
  if(size != 1)
  	return -EINVAL;
  
    wait_event_interruptible(button_waitq, ev_press);
  
  copy_to_user(buf,&key_val,1);    //如有按键按下，将键值传到用户程序

  ev_press = 0;
  return 1;
}


       static int fifth_drv_close (struct inode *inode, struct file *file)
{
        free_irq(IRQ_EINT0, &pins_desc[0]);
		free_irq(IRQ_EINT2, &pins_desc[1]);
		free_irq(IRQ_EINT11,&pins_desc[2]);
		free_irq(IRQ_EINT19,&pins_desc[3]);
 		return 0;
}
	static ssize_t fifth_dev_write(struct file *file, const char __user *buf, size_t size, loff_t * ppos)
{
  return 0;
}

	static	 unsigned fifth_drv_poll(struct file *file, poll_table *wait)
			{
				unsigned int mask = 0;
				
				poll_wait(file,&button_waitq,wait);
	
				if(ev_press)
					mask |= POLLIN|POLLRDNORM;
				 
				return mask;
			}  
	static int fifth_drv_fasync (int fd, struct file *filp, int on)
	
	{
        printk("driver:fifth_drv_fasync\n");
		return fasync_helper (fd, filp, on, &button_async);
	}


	static struct file_operations fifth_dev_fops = {
			.owner	=	THIS_MODULE,	
			.open	=	fifth_dev_open,		   
			.write	=	fifth_dev_write,	 
			.read	=	fifth_dev_read,
			.release=   fifth_drv_close,
			.poll   =   fifth_drv_poll,
			.fasync =   fifth_drv_fasync, 
		};
			
		

static int fifth_drv_init(void)
{
  major = register_chrdev(0,"fifth_drv", &fifth_dev_fops);
  
  fifthdrv_class = class_create(THIS_MODULE, "fifth_drv");

  fifthdrv_class_dev = class_device_create(fifthdrv_class, NULL, MKDEV(major, 0), NULL, "buttons");   // /dev/buttons
  
  gpfcon = (volatile unsigned long*)ioremap(0x56000050,16);

  gpfdat = gpfcon+1;

  gpgcon = (volatile unsigned long*)ioremap(0x56000060,16);

  gpgdat = gpgcon+1;
  
  return 0;
}

static int fifth_drv_exit(void)
{
  major = unregister_chrdev(major,"fifth_drv");
  class_device_unregister(fifthdrv_class_dev);
  class_destroy(fifthdrv_class);

  iounmap(gpfcon);
  iounmap(gpgcon);
  return 0;
}



module_init(fifth_drv_init);
module_exit(fifth_drv_exit);

MODULE_LICENSE("GPL");

