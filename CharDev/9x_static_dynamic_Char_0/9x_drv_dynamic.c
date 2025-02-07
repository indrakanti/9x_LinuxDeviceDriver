#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <asm/current.h>
#include <linux/slab.h>

#define CHAR_DRV9X "9x_cdrv"
#define MAX_LENGTH 4000
#define SUCCESS 0
#define FIRSTMINOR 0

static char *device_buffer;
struct cdev *char_drv9x;
dev_t device_id;

static int char_dev_open(struct inode *inode, struct file  *file)
{
    printk(KERN_INFO "Open operation invoked \n");
    return SUCCESS;
}

static int char_dev_release(struct inode *inode, struct file *file)
{
    return SUCCESS;
}

static ssize_t char_dev_write(struct file *file, const char *buf, size_t lbuf, loff_t *ppos)
{
    int nbytes;

    nbytes = lbuf - copy_from_user(device_buffer + *ppos, buf, lbuf);
    *ppos += nbytes;
    printk(KERN_INFO "Received from App data %s of length %d \n", device_buffer, nbytes);
    return nbytes;
}

static struct file_operations char_dev_fops = {
    .owner = THIS_MODULE,
    .write = char_dev_write,
    .open = char_dev_open,
    .release = char_dev_release,
};

static __init int char_dev_init(void)
{
    int ret, count = 1;

    if (alloc_chrdev_region(&device_id, 0, count, CHAR_DRV9X) < 0) {
        printk(KERN_ERR "Failed to reserve major/minor range\n");
        return -1;
    }

    if (!(char_drv9x = cdev_alloc())) {
        printk(KERN_ERR "cdev_alloc() failed\n");
        unregister_chrdev_region(device_id, count);
        return -1;
    }
    cdev_init(char_drv9x, &char_dev_fops);

    ret = cdev_add(char_drv9x, device_id, count);
    if (ret < 0) {
        printk(KERN_INFO "Error registering device driver\n");
        cdev_del(char_drv9x);
        unregister_chrdev_region(device_id, count);
        return -1;
    }
    printk(KERN_INFO "\nDevice Registered: %s\n", CHAR_DRV9X);
    printk(KERN_INFO "Major number = %d, Minor number = %d\n", MAJOR(device_id), MINOR(device_id));

    device_buffer = (char *)kmalloc(MAX_LENGTH, GFP_KERNEL);
    return 0;
}

static __exit void char_dev_exit(void)
{
    cdev_del(char_drv9x);
    unregister_chrdev_region(device_id, 1);
    kfree(device_buffer);
    printk(KERN_INFO "\nDriver unregistered\n");
}
module_init(char_dev_init);
module_exit(char_dev_exit);

MODULE_AUTHOR("Ram Indrakanti");
MODULE_DESCRIPTION("Character Device Driver - Test");
MODULE_LICENSE("MIT");
