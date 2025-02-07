#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/slab.h>

#define MAJORNO 300
#define MINORNO  0
#define CHAR_DRV9X "9x_cdrv"
#define MAX_LENGTH 4000
#define SUCCESS 0

static char device_buffer[MAX_LENGTH];
static struct cdev *char_drv9x;
static dev_t device_id;
int count=1,inuse=0;

static int char_dev_open(struct inode *inode, struct file  *file)
{
    if(inuse)
    {
        printk(KERN_INFO "\nDevice busy %s\n", CHAR_DRV9X);
        return -EBUSY;
    }
    inuse=1;
    printk(KERN_INFO "Open operation invoked \n");
    return SUCCESS;
}

static int char_dev_release(struct inode *inode, struct file *file)
{
    inuse=0;
    return SUCCESS;
}

static ssize_t char_dev_write(struct file *file, const char *buf, size_t lbuf, loff_t *ppos)
{
    int nbytes = lbuf - copy_from_user(device_buffer + *ppos, buf, lbuf);
    *ppos += nbytes;
    printk(KERN_INFO "\nReceived data from app %s , nbytes=%d\n", device_buffer, nbytes);
    return nbytes;
}

static struct file_operations char_dev_fops = {
    .owner = THIS_MODULE,
    .write = char_dev_write,
    .open = char_dev_open,
    .release = char_dev_release
};

static int __init char_dev_init(void)
{
    int ret;
    device_id = MKDEV(MAJORNO, MINORNO);
    register_chrdev_region(device_id, count, CHAR_DRV9X);

    char_drv9x = cdev_alloc();
    cdev_init(char_drv9x, &char_dev_fops);

    ret = cdev_add(char_drv9x, device_id, count);
    if(ret < 0) {
        printk(KERN_INFO "Error registering device driver\n");
        return ret;
    }
    printk(KERN_INFO "\nDevice Registered %s\n", CHAR_DRV9X);
    memset(device_buffer, '\0', MAX_LENGTH);
    return 0;
}

static void __exit char_dev_exit(void)
{
    cdev_del(char_drv9x);
    unregister_chrdev_region(device_id, 1);
    printk(KERN_INFO "\nDriver unregistered\n");
}

module_init(char_dev_init);
module_exit(char_dev_exit);

MODULE_AUTHOR("Ram Indrakanti");
MODULE_DESCRIPTION("Character Device Driver - Test");
MODULE_LICENSE("MIT");
