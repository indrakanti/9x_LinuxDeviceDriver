/* High-Performance Character Device Driver Using Spinlocks */

#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/atomic.h>
#include <linux/spinlock.h>
#include <linux/delay.h>
#include <linux/slab.h>

#define DEVICE_NAME "fast_cdrv"
#define BUFFER_SIZE 1024

static DEFINE_SPINLOCK(buffer_lock);
static char *device_buffer;
static struct cdev *device_cdev;
dev_t dev_number;
static struct class *device_class;

static int device_open(struct inode *inode, struct file *file) {
    static atomic_t open_counter = ATOMIC_INIT(0);
    atomic_inc(&open_counter);
    pr_info("Device opened %d times\n", atomic_read(&open_counter));
    return 0;
}

static ssize_t device_read(struct file *file, char __user *buf, size_t len, loff_t *offset) {
    int available = BUFFER_SIZE - *offset;
    int to_read = min(len, available);
    if (to_read == 0) return -ENOSPC;
    
    spin_lock(&buffer_lock);
    int bytes_copied = to_read - copy_to_user(buf, device_buffer + *offset, to_read);
    spin_unlock(&buffer_lock);
    
    *offset += bytes_copied;
    return bytes_copied;
}

static ssize_t device_write(struct file *file, const char __user *buf, size_t len, loff_t *offset) {
    int available = BUFFER_SIZE - *offset;
    int to_write = min(len, available);
    if (to_write == 0) return -ENOSPC;
    
    spin_lock(&buffer_lock);
    int bytes_written = to_write - copy_from_user(device_buffer + *offset, buf, to_write);
    spin_unlock(&buffer_lock);
    
    *offset += bytes_written;
    return bytes_written;
}

static struct file_operations device_fops = {
    .owner = THIS_MODULE,
    .read = device_read,
    .write = device_write,
    .open = device_open,
};

static int __init device_init(void) {
    if (alloc_chrdev_region(&dev_number, 0, 1, DEVICE_NAME) < 0) return -1;
    device_cdev = cdev_alloc();
    cdev_init(device_cdev, &device_fops);
    cdev_add(device_cdev, dev_number, 1);
    device_class = class_create(THIS_MODULE, "fast_device_class");
    device_create(device_class, NULL, dev_number, NULL, DEVICE_NAME);
    device_buffer = kzalloc(BUFFER_SIZE, GFP_KERNEL);
    return 0;
}

static void __exit device_exit(void) {
    device_destroy(device_class, dev_number);
    class_destroy(device_class);
    cdev_del(device_cdev);
    unregister_chrdev_region(dev_number, 1);
    kfree(device_buffer);
}

module_init(device_init);
module_exit(device_exit);

MODULE_AUTHOR("Kernel Dev");
MODULE_DESCRIPTION("High-Performance Spinlock-Based Character Device Driver");
MODULE_LICENSE("GPL");
