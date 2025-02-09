#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <asm/current.h>
#include <linux/device.h>
#include <linux/slab.h>

/* Character Device Definitions */
#define CHAR_DEV_NAME "9x_cdrv"
#define MAX_LENGTH 4000
#define SUCCESS 0

static char *9xChar_device_buf;
struct cdev *9xChar_cdev;
dev_t mydev;
int count = 1;
static struct class *9xChar_class;

/* Open the device */
static int 9xChar_dev_open(struct inode *inode, struct file *file)
{
	static int counter = 0;
	counter++;
	printk(KERN_INFO "Number of times open() was called: %d\n", counter);
	printk(KERN_INFO " MAJOR number = %d, MINOR number = %d\n", imajor(inode), iminor(inode));
	printk(KERN_INFO "Process id of the current process: %d\n", current->pid);
	printk(KERN_INFO "ref=%d\n", module_refcount(THIS_MODULE));
	return SUCCESS;
}

/* Close the device */
static int 9xChar_dev_release(struct inode *inode, struct file *file)
{
	return SUCCESS;
}

/* Read from the device */
static ssize_t 9xChar_dev_read(struct file *file, char *buf, size_t lbuf, loff_t *ppos)
{
	int maxbytes;
	int bytes_to_do;
	int nbytes;

	maxbytes = MAX_LENGTH - *ppos;

	if (maxbytes > lbuf)
		bytes_to_do = lbuf;
	else
		bytes_to_do = maxbytes;

	if (bytes_to_do == 0) {
		printk("Reached end of device\n");
		return -ENOSPC;
	}

	nbytes = bytes_to_do - copy_to_user(buf, 9xChar_device_buf + *ppos, bytes_to_do);
	*ppos += nbytes;
	return nbytes;
}

/* Write to the device */
static ssize_t 9xChar_dev_write(struct file *file, const char *buf, size_t lbuf, loff_t *ppos)
{
	int nbytes;
	int bytes_to_do;
	int maxbytes;

	maxbytes = MAX_LENGTH - *ppos;

	if (maxbytes > lbuf)
		bytes_to_do = lbuf;
	else
		bytes_to_do = maxbytes;

	if (bytes_to_do == 0) {
		printk("Reached end of device\n");
		return -ENOSPC;
	}

	nbytes = bytes_to_do - copy_from_user(9xChar_device_buf + *ppos, buf, bytes_to_do);
	*ppos += nbytes;
	return nbytes;
}

/* Seek within the device */
static loff_t 9xChar_dev_lseek(struct file *file, loff_t offset, int orig)
{
	loff_t testpos;

	switch (orig) {
	case 0:
		testpos = offset;
		break;
	case 1:
		testpos = file->f_pos + offset;
		break;
	case 2:
		testpos = MAX_LENGTH + offset;
		break;
	default:
		return -EINVAL;
	}

	testpos = testpos < MAX_LENGTH ? testpos : MAX_LENGTH;
	testpos = testpos >= 0 ? testpos : 0;
	file->f_pos = testpos;
	printk(KERN_INFO "Seeking to pos=%ld\n", (long)testpos);
	return testpos;
}

/* File operations structure */
static struct file_operations 9xChar_dev_fops = {
	.owner = THIS_MODULE,
	.read = 9xChar_dev_read,
	.write = 9xChar_dev_write,
	.open = 9xChar_dev_open,
	.release = 9xChar_dev_release,
	.llseek = 9xChar_dev_lseek
};

/* Initialize the device */
static __init int 9xChar_dev_init(void)
{
	int ret;

	if (alloc_chrdev_region(&mydev, 0, count, CHAR_DEV_NAME) < 0) {
		printk(KERN_ERR "Failed to reserve major/minor range\n");
		return -1;
	}

	if (!(9xChar_cdev = cdev_alloc())) {
		printk(KERN_ERR "cdev_alloc() failed\n");
		unregister_chrdev_region(mydev, count);
		return -1;
	}

	cdev_init(9xChar_cdev, &9xChar_dev_fops);
	ret = cdev_add(9xChar_cdev, mydev, count);

	if (ret < 0) {
		printk(KERN_INFO "Error registering device driver\n");
		cdev_del(9xChar_cdev);
		unregister_chrdev_region(mydev, count);
		return -1;
	}

	9xChar_class = class_create(THIS_MODULE, "VIRTUAL");
	device_create(9xChar_class, NULL, mydev, NULL, "%s", "9x_cdrv");

	printk(KERN_INFO "\nDevice Registered: %s\n", CHAR_DEV_NAME);
	printk(KERN_INFO "Major number = %d, Minor number = %d\n", MAJOR(mydev), MINOR(mydev));

	9xChar_device_buf = (char *)kmalloc(MAX_LENGTH, GFP_KERNEL);
	return 0;
}

/* Cleanup module */
static __exit void 9xChar_dev_exit(void)
{
	device_destroy(9xChar_class, mydev);
	class_destroy(9xChar_class);
	cdev_del(9xChar_cdev);
	unregister_chrdev_region(mydev, 1);
	kfree(9xChar_device_buf);
	printk(KERN_INFO "\nDriver unregistered\n");
}

module_init(9xChar_dev_init);
module_exit(9xChar_dev_exit);

MODULE_AUTHOR("Ram Indrakanti");
MODULE_DESCRIPTION("9x Character Device Driver");
MODULE_LICENSE("GPL");
