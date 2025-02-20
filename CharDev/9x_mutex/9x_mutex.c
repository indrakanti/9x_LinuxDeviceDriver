#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/cdev.h>
#include <linux/sched.h>
#include <linux/errno.h>
#include <linux/device.h>
#include <linux/atomic.h>
#include <linux/mutex.h>
#include <linux/delay.h>

#define CHAR_DEV_NAME "mutex_cdrv"
#define MAX_LENGTH 255
#define SUCCESS 0

DEFINE_MUTEX(mylock);
static char *char_device_buf;
struct cdev *my_cdev;
dev_t mydev;
int count = 1;
static struct class *my_class;
static int recursive_lock(void);
static int accidental_release(void);

static int char9x_dev_open(struct inode *inode, struct file *file) {
    static atomic_t counter = ATOMIC_INIT(0);
    atomic_inc(&counter);
    pr_info("Device opened %d times\n", atomic_read(&counter));
    pr_info("MAJOR: %d, MINOR: %d\n", imajor(inode), iminor(inode));
    pr_info("Process ID: %d\n", current->pid);
    return SUCCESS;
}

static int char9x_dev_release(struct inode *inode, struct file *file) {
    return SUCCESS;
}

static ssize_t char9x_dev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos) {
    int maxbytes = MAX_LENGTH - *ppos;
    int bytes_to_do = (maxbytes > lbuf) ? lbuf : maxbytes;
    if (bytes_to_do == 0) {
        pr_info("End of device reached\n");
        return -ENOSPC;
    }
    pr_debug("Reader acquiring lock\n");
    if (mutex_lock_interruptible(&mylock)) return -ERESTARTSYS;
    pr_debug("Reader executing critical code\n");
    int nbytes = bytes_to_do - copy_to_user(buf, char9x_device_buf + *ppos, bytes_to_do);
    mutex_unlock(&mylock);
    *ppos += nbytes;
    return nbytes;
}

static ssize_t char9x_dev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos) {
    int maxbytes = MAX_LENGTH - *ppos;
    int bytes_to_do = (maxbytes > lbuf) ? lbuf : maxbytes;
    if (bytes_to_do == 0) {
        pr_info("End of device reached\n");
        return -ENOSPC;
    }
    if (mutex_lock_interruptible(&mylock)) return -ERESTARTSYS;
    pr_debug("Writer acquiring lock\n");
    recursive_lock();
    pr_debug("Writer executing critical code\n");
    int nbytes = bytes_to_do - copy_from_user(char9x_device_buf + *ppos, buf, bytes_to_do);
    ssleep(30);
    pr_debug("Writer releasing lock\n");
    mutex_unlock(&mylock);
    *ppos += nbytes;
    return nbytes;
}

static loff_t char9x_dev_lseek(struct file *file, loff_t offset, int orig) {
    loff_t testpos;
    switch (orig) {
    case SEEK_SET: testpos = offset; break;
    case SEEK_CUR: testpos = file->f_pos + offset; break;
    case SEEK_END: testpos = MAX_LENGTH + offset; break;
    default: return -EINVAL;
    }
    testpos = (testpos < MAX_LENGTH) ? testpos : MAX_LENGTH;
    testpos = (testpos >= 0) ? testpos : 0;
    file->f_pos = testpos;
    pr_info("Seeking to position: %ld\n", (long)testpos);
    return testpos;
}

static const struct file_operations char9x_dev_fops = {
    .owner = THIS_MODULE,
    .read = char9x_dev_read,
    .write = char9x_dev_write,
    .open = char9x_dev_open,
    .release = char9x_dev_release,
    .llseek = char9x_dev_lseek
};

static __init int char9x_dev_init(void) {
    int ret;
    if (alloc_chrdev_region(&mydev, 0, count, char9x_DEV_NAME) < 0) {
        pr_err("Failed to reserve major/minor range\n");
        return -1;
    }
    my_cdev = cdev_alloc();
    if (!my_cdev) {
        pr_err("cdev_alloc() failed\n");
        unregister_chrdev_region(mydev, count);
        return -1;
    }
    cdev_init(my_cdev, &char9x_dev_fops);
    ret = cdev_add(my_cdev, mydev, count);
    if (ret < 0) {
        pr_err("Error registering device driver\n");
        cdev_del(my_cdev);
        unregister_chrdev_region(mydev, count);
        return -1;
    }
    my_class = class_create(THIS_MODULE, "VIRTUAL");
    device_create(my_class, NULL, mydev, NULL, "%s", "mutex_cdrv");
    pr_info("Device Registered: %s\n", char9x_DEV_NAME);
    char9x_device_buf = kzalloc(MAX_LENGTH, GFP_KERNEL);
    return 0;
}

static __exit void char9x_dev_exit(void) {
    device_destroy(my_class, mydev);
    class_destroy(my_class);
    cdev_del(my_cdev);
    unregister_chrdev_region(mydev, 1);
    kfree(char9x_device_buf);
    pr_info("Driver unregistered\n");
}

module_init(char9x_dev_init);
module_exit(char9x_dev_exit);

MODULE_AUTHOR("Ram Indrakanti");
MODULE_DESCRIPTION("Mutex-based Character Device Driver - 9x");
MODULE_LICENSE("GPL");
