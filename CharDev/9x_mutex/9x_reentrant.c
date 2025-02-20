/*need this only for reentrant behaviour*/
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/atomic.h>
#include <linux/semaphore.h>
#include <linux/delay.h>
#include <linux/slab.h>

#define CHAR_DEV_NAME "reentrant_cdrv"
#define MAX_LENGTH 255
#define SUCCESS 0

static DEFINE_SEMAPHORE(mysem);
static char *char_device_buf;
static struct cdev *char_cdev;
dev_t mydev;
static struct class *char_class;

static int char_dev_open(struct inode *inode, struct file *file) {
    static atomic_t counter = ATOMIC_INIT(0);
    atomic_inc(&counter);
    pr_info("Device opened %d times\n", atomic_read(&counter));
    return SUCCESS;
}

static ssize_t char_dev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
    int maxbytes = MAX_LENGTH - *ppos;
    int bytes_to_do = min(lbuf, maxbytes);
    if (bytes_to_do == 0) return -ENOSPC;
    
    pr_debug("Reader acquiring semaphore\n");
    if (down_interruptible(&mysem)) return -ERESTARTSYS;
    
    int nbytes = bytes_to_do - copy_to_user(buf, char_device_buf + *ppos, bytes_to_do);
    up(&mysem);
    *ppos += nbytes;
    return nbytes;
}

static ssize_t char_dev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos) {
    int maxbytes = MAX_LENGTH - *ppos;
    int bytes_to_do = min(lbuf, maxbytes);
    if (bytes_to_do == 0) return -ENOSPC;
    
    pr_debug("Writer acquiring semaphore\n");
    if (down_interruptible(&mysem)) return -ERESTARTSYS;
    
    int nbytes = bytes_to_do - copy_from_user(char_device_buf + *ppos, buf, bytes_to_do);
    ssleep(30);
    pr_debug("Writer releasing semaphore\n");
    up(&mysem);
    *ppos += nbytes;
    return nbytes;
}

static struct file_operations char_dev_fops = {
    .read = char_dev_read,
    .write = char_dev_write,
    .open = char_dev_open,
};

static __init int char_dev_init(void) {
    if (alloc_chrdev_region(&mydev, 0, 1, CHAR_DEV_NAME) < 0) return -1;
    char_cdev = cdev_alloc();
    cdev_init(char_cdev, &char_dev_fops);
    cdev_add(char_cdev, mydev, 1);
    char_class = class_create(THIS_MODULE, "char_class");
    device_create(char_class, NULL, mydev, NULL, CHAR_DEV_NAME);
    char_device_buf = kzalloc(MAX_LENGTH, GFP_KERNEL);
    return 0;
}

static __exit void char_dev_exit(void) {
    device_destroy(char_class, mydev);
    class_destroy(char_class);
    cdev_del(char_cdev);
    unregister_chrdev_region(mydev, 1);
    kfree(char_device_buf);
}

module_init(char_dev_init);
module_exit(char_dev_exit);

MODULE_AUTHOR("Updated");
MODULE_DESCRIPTION("Fully Reentrant Character Device Driver using Semaphores");
MODULE_LICENSE("GPL");
