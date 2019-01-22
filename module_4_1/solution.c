#include <linux/kobject.h>
#include <linux/string.h>
#include <linux/sysfs.h>
#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/list.h>
#include <linux/list_sort.h>

struct my_module_list
{
    struct list_head list;
    char name[128];
};

static struct my_module_list my_list;

static int cmp(void *priv, struct list_head *a, struct list_head *b)
{
    struct my_module_list *ela, *elb;
    
    ela = container_of(a, struct my_module_list, list);
    elb = container_of(b, struct my_module_list, list);

    return strcmp(ela->name, elb->name);
}

static void get_modules(void)
{
    struct module * mod;
    struct list_head * pos;

    struct my_module_list *new_data;

    INIT_LIST_HEAD(&my_list.list);

    while(!mutex_trylock(&module_mutex))
        cpu_relax();

    list_for_each(pos, THIS_MODULE->list.prev) 
    {
        bool head = (unsigned long)pos >= MODULES_VADDR;
            
        mod = container_of(pos, struct module, list);

        new_data = kmalloc(sizeof(struct my_module_list), GFP_KERNEL);
        if (head)  
            snprintf(new_data->name, 64, "%s", mod->name);
        
        list_add(&new_data->list, &my_list.list);
    }

    mutex_unlock(&module_mutex);
}

static void list_modules(void)
{
    struct list_head * pos;
    struct my_module_list *ptr;

    pr_info("List of available modules:\n");

    list_sort(NULL, &my_list.list, cmp);

    list_for_each(pos, &my_list.list)
    {
        ptr = list_entry(pos, struct my_module_list, list);
    
        pr_info("%s\n", ptr->name);
    }
}
/*
static void clear_modules(void)
{
    struct list_head * pos;
    struct my_module_list *ptr;
        
    list_for_each(pos, &my_list.list)
    {
        ptr = list_entry(pos, struct my_module_list, list);
        list_del(pos);
        kfree(ptr);
    }
}*/

static ssize_t foo_show(struct kobject *kobj, struct kobj_attribute *attr, char *buf)
{
    struct list_head * pos;
    struct my_module_list *ptr;
    int size = 0;
    
    list_for_each(pos, &my_list.list)
    {
        ptr = list_entry(pos, struct my_module_list, list);
        size += sprintf(buf + size, "%s\n", ptr->name);
    }

	return size;
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
	int retval;

    get_modules();
    list_modules();

	example_kobj = kobject_create_and_add("my_kobject", kernel_kobj);
	if (!example_kobj)
		return -ENOMEM;

	/* Create the files associated with this kobject */
	retval = sysfs_create_group(example_kobj, &attr_group);
	if (retval)
		kobject_put(example_kobj);

	return retval;
}

static void __exit example_exit(void)
{
	kobject_put(example_kobj);
}

module_init(example_init);
module_exit(example_exit);
MODULE_LICENSE("GPL v2");
MODULE_AUTHOR("Rasskazov Egor <ranihilator@gmail.com>");
