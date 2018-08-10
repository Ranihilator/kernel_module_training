#include <linux/module.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/ioctl.h>

#define KBUF_SIZE (10 * PAGE_SIZE)

#define IOC_MAGIC 'k'

#define SUM_LENGTH _IOWR(IOC_MAGIC, 1, char*)
#define SUM_CONTENT _IOWR(IOC_MAGIC, 2, char*)

static dev_t first;
static unsigned int count = 1;
static struct cdev *my_cdev;

char *MYDEV_NAME = "solution_node";

static char *node_name = NULL;

module_param(node_name, charp, 0000);
MODULE_PARM_DESC(node_name, "A character string");

static struct class *my_class;

static int M_size = 0, N_size = 0;
static long M = 0, N = 0;

static long mychrde_ioctl(struct file *file, unsigned int command, unsigned long argument)
{
    char* arg = (char*)argument;
    int size;
    int i;
    unsigned long content;

    if (!arg)
        return -EINVAL;

    size = strlen(arg);

    switch (command)
    {
        case SUM_LENGTH:
            for (i = 0; i < size; ++i)
                M += arg[i];
            
            M_size += size;
            
            return M_size;
            break;
        
        case SUM_CONTENT:
            kstrtoul(arg, 10, &content);
            N += content;
    
            N_size += size;

            return N;
            break;
        
        default:
            return -EINVAL;
    }

    return 0;
}

static int mychrdev_open(struct inode *inode, struct file *file)
{
    char *kbuf = kcalloc(KBUF_SIZE, sizeof(char), GFP_KERNEL);
    file->private_data = kbuf;

    printk(KERN_INFO "Opening device %s:\n\n", MYDEV_NAME);

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

	return 0;
}

static ssize_t mychrdev_read(struct file *file, char __user *buf, size_t lbuf, loff_t *ppos)
{
	char *kbuf = file->private_data;
	ssize_t nbytes = 0;

	sprintf(kbuf, "%u %lu\n", M_size, N);

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
	ssize_t nbytes = lbuf;
	*ppos += nbytes;

	printk(KERN_INFO "Write dev %s nbytes = %lu, ppos = %d:\n\n", MYDEV_NAME, nbytes, (int)*ppos);
	return nbytes;
}

static loff_t mychrdev_lseek(struct file *file, loff_t offset, int orig)
{
	loff_t testpos;

	switch (orig)
	{
		case SEEK_SET:
			testpos = offset;
			break;

		case SEEK_CUR:
			testpos = file->f_pos + offset;
			break;

		case SEEK_END:
			testpos = KBUF_SIZE + offset;
			break;

		default:
			return -EINVAL;
	}

	testpos = testpos < KBUF_SIZE ? testpos : KBUF_SIZE;
	testpos = testpos >=0 ? testpos : 0;

	file->f_pos = testpos;

	printk(KERN_INFO "Seeking to %lu position\n", (long)testpos);

	return testpos;
}

static const struct file_operations mycdev_fops =
{
	.owner = THIS_MODULE,
	.read = mychrdev_read,
	.write = mychrdev_write,
	.open = mychrdev_open,
	.release = mychrdev_release,
	.llseek = mychrdev_lseek,
	.unlocked_ioctl = mychrde_ioctl
};

static int __init init_chrdev(void)
{
	if (node_name)
		MYDEV_NAME = node_name;

	pr_info("Try to loading module %s\n", MYDEV_NAME);

	if (alloc_chrdev_region(&first, 0, 1, MYDEV_NAME) < 0)
	{
		pr_err("Loading failed at getting ID device class\n");
		return -EINVAL;
	}

	if ((my_class = class_create(THIS_MODULE, MYDEV_NAME)) == NULL)
	{
		pr_err("Loading failed at creating class\n");
		unregister_chrdev_region(first, 1);
		return -EINVAL;
	}

	if (device_create(my_class, NULL, first, NULL, MYDEV_NAME) == NULL)
	{
		pr_err("Loading failed at device creating\n");
		class_destroy(my_class);
		unregister_chrdev_region(first, 1);
		return -EINVAL;
	}

	if ((my_cdev = cdev_alloc()) == NULL)
	{
		pr_err("Loading failed at device creating\n");
		device_destroy(my_class, first);
		class_destroy(my_class);
		unregister_chrdev_region(first, 1);
		return -EINVAL;
	}

	cdev_init(my_cdev, &mycdev_fops);
	if (cdev_add(my_cdev, first, 1) < 0)
	{
		device_destroy(my_class, first);
		class_destroy(my_class);
		unregister_chrdev_region(first, 1);
		return -EINVAL;
	}

	printk(KERN_INFO "Created device class %s with major=%i minor=%i\n", MYDEV_NAME, MAJOR(first), MINOR(first));

	return 0;
}

static void __exit cleanup_chrdev(void)
{
	printk(KERN_INFO "Destroy device %s\n", MYDEV_NAME);

	device_destroy(my_class, first);
	class_destroy(my_class);

	if (my_cdev)
		cdev_del(my_cdev);

	unregister_chrdev_region(first, count);
}

module_init(init_chrdev);
module_exit(cleanup_chrdev);

MODULE_LICENSE("GPL");
