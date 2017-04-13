#include <linux/ioctl.h> //Needed for the _IOW stuff used later 
#define SCULL_IOC_MAGIC 'k'
#define SCULL_HELLO_IO(SCULL_IOC_MAGIC, 1)

long ioctl_example (struct file *filp, unsigned int cmd, unsignd long arg)
{
	int err = 0, tmp;
	int retval = 0;
	
	/*
		extract the type and number bitfields, and do not decode 
		wrong cmds: return ENOTTY (inappropriate ioctl) before access_ok()
	*/
	if (_IOC_TYPE(cmd) != SCULL_IOC_MAGIC) return -ENOTTY;
	if (_IOC_NR(cmd) > SCULL_IOC_MAXNR) return -ENOTTY;
	
	/*
		The direction is a bitmask, and VERIFY_WRITE catchs R/W transfers.
		'Type' is user-oriented, while access_ok is kernel-oriented, so
		the concept of "read" and "write" is reversed.
	*/
	if (_IOC_DIR(cmd) & _IOC_READ)
		err = !access_ok(VERIFY_WRITE, (void __user *)arg, _IOC_SIZE(cmd));
	else if (_IOC_DIR(cmd) & _IOC_WRITE)
		err = !access_ok(VERIFY_READ, (void __user *)arg, _IOC_SIZE(cmd));
	
	if (err) return -EFAULT;
	
	switch (cmd) {
		case SCULL_HELLO:
			printk(KERN_WARNING "hello\n");
			break;
		default: /* redundant, as cmd was checked against MAXNR */
			return -ENOTTY;
	}
	
	return retval;
}
