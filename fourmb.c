#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#define MAJOR_NUMBER 61
#define FOUR_MEGABYTES 4194304

/* forward declaration */
int fourmb_open(struct inode *inode, struct file *filep);
int fourmb_release(struct inode *inode, struct file *filep);
ssize_t fourmb_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t fourmb_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
static void fourmb_exit(void);

/* definition of file_operation structure */
struct file_operations fourmb_fops = {
	read: fourmb_read,
	write: fourmb_write,
	open: fourmb_open,
	release: fourmb_release
};


char* fourmb_data = NULL;
long data_length = 0;
long iRead = 0;
long iWrite = 0;

int fourmb_open(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}


int fourmb_release(struct inode *inode, struct file *filep)
{
	return 0; // always successful
}

ssize_t fourmb_read(struct file *filep, char *buf, size_t count, loff_t *f_pos)
{	
	if (iRead == data_length)
	{
		iRead = 0;
		return 0;
	}

	
	copy_to_user(buf, &fourmb_data[iRead], 1);
	iRead = iRead + 1;

	//printk(KERN_ALERT "iRead: %i, iWrite: %i, data_length: %i \n", iRead, iWrite, data_length);

	return 1;
}

ssize_t fourmb_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{

	if (iWrite >= FOUR_MEGABYTES)
	{				
		iWrite++;
		
		if (buf[0] == '\0')
			iWrite = 0;

		return -ENOSPC;
	}

	if (count == 1 && (buf[0] == '\0' || buf[0] == '\n'))
	{
		iWrite = 0;		
		return data_length;		
	}

	
	if (iWrite < FOUR_MEGABYTES)
	{
		copy_from_user(&fourmb_data[iWrite], buf, 1);	
		iWrite = iWrite + 1;
		data_length = iWrite;
	}
		

	//printk(KERN_ALERT "iWrite: %i, buffer: %c, count: %d \n", iWrite, buf[0], count);

	return 1;
}

static int fourmb_init(void)
{
	int result;
	// register the device
	result = register_chrdev(MAJOR_NUMBER, "fourmb", &fourmb_fops);
	if (result < 0) {
		return result;
	}

	// allocate one byte of memory for storage
	// kmalloc is just like malloc, the second parameter is
	// the type of memory to be allocated.
	// To release the memory allocated by kmalloc, use kfree.
	fourmb_data = kmalloc(sizeof(char) * FOUR_MEGABYTES, GFP_KERNEL);

	if (!fourmb_data) {
		fourmb_exit();
		
		// cannot allocate memory
		// return no memory error, negative signify a failure
		return -ENOMEM;
	}	

	printk(KERN_ALERT "This is a four megabytes device module\n");

	return 0;
}

static void fourmb_exit(void)
{
	// if the pointer is pointing to something
	if (fourmb_data) {
		// free the memory and assign the pointer to NULL
		kfree(fourmb_data);
		fourmb_data = NULL;
	}
	
	// unregister the device
	unregister_chrdev(MAJOR_NUMBER, "fourmb");
	printk(KERN_ALERT "Four megabytes device module is unloaded\n");
}

MODULE_LICENSE("GPL");
module_init(fourmb_init);
module_exit(fourmb_exit);
