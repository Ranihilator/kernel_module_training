#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>

static dev_t first;
static unsigned int count = 1;
static int my_major = 240, my_minor = 0;
static struct cdev *my_cdev;

#define MYDEV_NAME "mychrdev"
#define KBUF_SIZE (10 * PAGE_SIZE)

static int counter = 0;
static size_t byte_counter = 0;

static int mychrdev_open(struct inode *inode, struct file *file)
{
    char *kbuf = kcalloc(KBUF_SIZE, sizeof(char), GFP_KERNEL);
    file->private_data = kbuf;
    
    printk(KERN_INFO "Opening device %s:\n\n", MYDEV_NAME);
    counter++;

    printk(KERN_INFO "counter: %d\n", counter);
    printk(KERN_INFO "module refcounter: %d\n", module_refcount(THIS_MODULE));
    
    return 0;
}

static int mychrdev_release(struct inode *inode, struct file *file)
{
    char *kbuf = file->private_data;

    printk(KERN_INFO "Close device %s:\n\n", MYDEV_NAME);

    printk(KERN_INFO "Free buffer");
    if (kbuf)
        kfree(kbuf);

    kbuf = NULL;
    file->private_data = NULL;
    //counter--;

    return 0;
}

static ssize_t mychrdev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
    char *kbuf = file->private_data;
    ssize_t nbytes = 0;

    sprintf(kbuf, "%i %lu\n", counter, byte_counter);
    
    if (*ppos >= KBUF_SIZE)
        return 0;
    
    if ((lbuf + *ppos) > KBUF_SIZE)
        lbuf = KBUF_SIZE - *ppos;
    
    nbytes = lbuf - copy_to_user(buf, kbuf + *ppos, lbuf);
    *ppos += nbytes;

    printk(KERN_INFO "Read device %s nbytes = %lu, ppos = %d:\n\n", MYDEV_NAME, nbytes, (int)*ppos);    

    return nbytes;
}

static ssize_t mychrdev_write(struct file *file, const char __user *buf, size_t lbuf, loff_t *ppos)
{
    //char *kbuf = file->private_data;
    ssize_t nbytes = lbuf;
    //ssize_t nbytes = lbuf - copy_from_user(kbuf + *ppos, buf, lbuf);
    *ppos += nbytes;
    byte_counter += *ppos;
    
    printk(KERN_INFO "Write dev %s nbytes = %lu, ppos = %d:\n\n", MYDEV_NAME, nbytes, (int)*ppos);
    return nbytes;
}

static const struct file_operations mycdev_fops = 
{
        .owner = THIS_MODULE,
        .read = mychrdev_read,
        .write = mychrdev_write,
        .open = mychrdev_open,
        .release = mychrdev_release
};

static int __init init_chrdev(void)
{
    printk(KERN_INFO "Loading\n");
    
    first = MKDEV (my_major, my_minor);
    register_chrdev_region(first, count, MYDEV_NAME);
    
    my_cdev = cdev_alloc();

    cdev_init(my_cdev, &mycdev_fops);
    cdev_add(my_cdev, first, count);
    
	return 0;
}

static void __exit cleanup_chrdev(void)
{
    printk(KERN_INFO "Leaving\n");

    if (my_cdev)
        cdev_del(my_cdev);

    unregister_chrdev_region(first, count);
}

module_init(init_chrdev);
module_exit(cleanup_chrdev);

MODULE_LICENSE("GPL");
