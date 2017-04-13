#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/errno.h>
#include <linux/types.h>
#include <linux/fs.h>
#include <linux/proc_fs.h>
#include <linux/ioctl.h> 
#include <asm/uaccess.h>

#define MAJOR_NUMBER 61
#define FOUR_MEGABYTES 4194304
#define FOURMB_IOC_MAGIC 'k'
#define FOURMB_IOC_MAXNR 4
#define FOURMB_HELLO _IO(FOURMB_IOC_MAGIC, 0)
#define FOURMB_WRITE_MSG _IOW(FOURMB_IOC_MAGIC, 1, int)
#define FOURMB_READ_MSG _IOR(FOURMB_IOC_MAGIC, 2, int)
#define FOURMB_WRITEREAD_MSG _IOWR(FOURMB_IOC_MAGIC, 3, int)


/* forward declaration */
int fourmb_open(struct inode *inode, struct file *filep);
int fourmb_release(struct inode *inode, struct file *filep);
ssize_t fourmb_read(struct file *filep, char *buf, size_t count, loff_t *f_pos);
ssize_t fourmb_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos);
loff_t fourmb_llseek(struct file *filep, loff_t offset, int origin);
long fourmb_ioctl (struct file * filep, unsigned int cmd, unsigned long arg);
static void fourmb_exit(void);

/* definition of file_operation structure */
struct file_operations fourmb_fops = {
	read: fourmb_read,
	write: fourmb_write,
	open: fourmb_open,
	release: fourmb_release,
	llseek: fourmb_llseek,
	unlocked_ioctl: fourmb_ioctl
};


char* fourmb_data = NULL;
long data_length = 0;


//IOCTL variables
char* dev_msg = NULL;

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
	if (*f_pos == data_length)
	{
		return 0;
	}

	
	copy_to_user(buf, &fourmb_data[*f_pos], 1);
	*f_pos = *f_pos + 1;


	return 1;
}

ssize_t fourmb_write(struct file *filep, const char *buf, size_t count, loff_t *f_pos)
{


	int i = 1;
	
	if (*f_pos < FOUR_MEGABYTES)
	{
		
		for (i = 0; i < count; i++)
		{
			if (*f_pos + i >= FOUR_MEGABYTES)
				break;

			copy_from_user(&fourmb_data[*f_pos+i], &buf[i], 1);	

			if (*f_pos + i + 1 > data_length)
			{
				data_length = *f_pos + i + 1;
			}
			
		}

		*f_pos += i;
	}
	else 
	{				
		*f_pos += count;

		return data_length;
	}
			
	return i;
}

loff_t fourmb_llseek(struct file *filep, loff_t offset, int origin) 
{	
	loff_t newpos;

	switch(origin) {
		case 0: /* SEEK_SET */
		newpos = offset;
		break;

		case 1: /* SEEK_CUR */
		newpos = filep->f_pos + offset;
		break;

		case 2: /* SEEK_END */
		newpos = data_length + offset;
		break;

		default: /* can't happen */
		return -EINVAL;
	}

	if (newpos < 0) return -EINVAL;
	filep->f_pos = newpos;
	return newpos;
}

long fourmb_ioctl (struct file * filep, unsigned int cmd, unsigned long arg) {
	int err = 0, i = 0;
	int retval = 0;
	int length = 0;
	char *tmpArg = kmalloc(sizeof(char) * 100, GFP_KERNEL);

	if (_IOC_TYPE(cmd) != FOURMB_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > FOURMB_IOC_MAXNR) return -ENOTTY;
	
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	
	if (err) return -EFAULT;
	
	switch (cmd) {
		case FOURMB_HELLO:
			printk(KERN_WARNING "hello\n");
			break;
		case FOURMB_WRITE_MSG:
			length = strlen((char *) arg);

			for (i = 0; i < length; i++)
			{
				retval = get_user(*(dev_msg+i), (char *)arg+i);
			}
			dev_msg[length] = '\0';
			break;
		case FOURMB_READ_MSG:
			length = strlen(dev_msg);
	
			for (i = 0; i < length; i++)
			{				
				retval = put_user(*(dev_msg+i), (char *)arg+i);

			}
			put_user('\0', (char *)arg+length);
			break;
		case FOURMB_WRITEREAD_MSG:

			// Step 1 - Move the dev_msg into temporary buffer			
			length = strlen(dev_msg);
			for (i = 0; i < length; i++) 
			{
				tmpArg[i] = dev_msg[i];
			}
			tmpArg[length] = '\0'; 

			// Step 2 - Move the value from user_msg into dev_msg
			length = strlen((char *) arg);
			for (i = 0; i < length; i++)
			{
				retval = get_user(*(dev_msg+i), (char *)arg+i);
			}
			dev_msg[length] = '\0';

			// Step 3 - Move the temporary buffer into user_msg
			length = strlen(tmpArg);	
			for (i = 0; i < length; i++)
			{				
				retval = put_user(*(tmpArg+i), (char *)arg+i);

			}
			put_user('\0', (char *)arg+length);

			printk("New dev_msg: %s \n", dev_msg);
		default: 
			return -ENOTTY;
	}
	
	return retval;
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
	dev_msg = kmalloc(sizeof(char) * FOUR_MEGABYTES, GFP_KERNEL);

	printk("I got: %zu bytes of memory\n", ksize(fourmb_data));

	if (!fourmb_data) {
		fourmb_exit();
		
		// cannot allocate memory
		// return no memory error, negative signify a failure
		return -ENOMEM;
	}	

	printk("This is a four megabytes device module\n");

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
