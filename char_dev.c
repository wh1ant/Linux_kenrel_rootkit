#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/interrupt.h>
#include <asm/io.h>
#include <linux/fcntl.h>
#include <linux/syscalls.h>
#include <linux/tcp.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/proc_fs.h>

#include "char_dev.h"

extern char buffer[100];

DECLARE_WAIT_QUEUE_HEAD(wait_queue_read);
int read_start = 5;
int mask = 0;



int poll_open(struct inode * inode, struct file * filp)
{
	return 0;
}



int poll_release(struct inode * inode, struct file * filp)
{
	return 0;
}



ssize_t poll_write(struct file * filp, const char * buf, size_t count, loff_t * f_pos)
{
	return count;
}



ssize_t poll_read(struct file * filp, char * buf, size_t count, loff_t * f_pos)
{
	return count;
}



ssize_t poll_poll(struct file * filp, struct poll_table_struct * wait)
{
	//int mask = 0;

	poll_wait(filp, &wait_queue_read, wait);
	//mask |= POLLIN | POLLRDNORM;

	//wake_up_interruptible(&wait_queue_read);
	//printk(KERN_ALERT "poll_poll call\n");

	if(mask!=0)
	{
		mask = 0;
		return 1;
	}

	return mask;  // zero result is block mode
}



static struct file_operations vd_fops = {
	.read = poll_read,
	.write = poll_write,
	.open = poll_open,
	.poll = poll_poll,
	.release = poll_release
};



void char_dev_init(void)
{
	if(register_chrdev(CHAR_DEV_MAJOR, CHAR_DEV, &vd_fops) < 0)
		printk(KERN_ERR "register_chrdev() error\n");
}



void char_dev_term(void)
{
	unregister_chrdev(CHAR_DEV_MAJOR, CHAR_DEV);
}
