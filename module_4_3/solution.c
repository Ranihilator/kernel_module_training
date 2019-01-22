#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/interrupt.h>
#include <linux/irq.h>
#include <linux/signal.h> 
#include <linux/sched.h> 

static int count_interrupts = 0;

static irqreturn_t handler(int irq, void *dev_id)
{
    count_interrupts++;

    return IRQ_HANDLED;
}

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
	return sprintf(buf, "%i\n", count_interrupts);;
}

static ssize_t foo_store(struct kobject *kobj, struct kobj_attribute *attr,
			 const char *buf, size_t count)
{
	return count;
}

/* Sysfs attributes cannot be world-writable. */
static struct kobj_attribute foo_attribute = __ATTR(my_sys, 0664, foo_show, foo_store);

/*
 * Create a group of attributes so that we can create and destroy them all
 * at once.
 */
static struct attribute *attrs[] = 
{
	&foo_attribute.attr,
	NULL,	/* need to NULL terminate the list of attributes */
};

/*
 * An unnamed attribute group will put all of the attributes directly in
 * the kobject directory.  If we specify a name, a subdirectory will be
 * created for the attributes with the directory being the name of the
 * attribute group.
 */
static struct attribute_group attr_group = 
{
	.attrs = attrs,
};

static struct kobject *example_kobj;

static int __init example_init(void)
{
	int retval = 0;
  // enable_irq(8);

    if (request_irq(8, handler, IRQF_SHARED, "my_rtc0", (void *)(handler)))
    {
        pr_err("rtc: cannot register IRQ %d\n", retval);
        return retval;
    }

    pr_info("Timer interrupt start\n");
    
	example_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
	if (!example_kobj)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(example_kobj, &attr_group);
	if (retval)
		kobject_put(example_kobj);

    pr_info("Kobject created\n");

    return retval;
}

static void __exit example_exit(void)
{
	kobject_put(example_kobj);
    free_irq(8, NULL);
    //disable_irq(8);
    pr_info("Module exit\n");
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Rasskazov Egor <ranihilator@gmail.com>");
