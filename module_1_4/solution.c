#include <linux/module.h>
#include <linux/device.h>
#include <linux/slab.h>

//#include "checker.h"

extern ssize_t get_void_size(void);
extern void submit_void_ptr(void *p);

extern ssize_t get_int_array_size(void);
extern void submit_int_array_ptr(int *p);

extern void submit_struct_ptr(struct device *p);
extern void checker_kfree(void *p);

void* ptr;
int* ptr_int;
struct device* ptr_dev;

int init_module(void)
{
    ptr = kmalloc(get_void_size(), GFP_KERNEL);
    submit_void_ptr(ptr);
    
    ptr_int = (int*)kmalloc(sizeof(int) * get_int_array_size(), GFP_KERNEL);
    submit_int_array_ptr(ptr_int);

    ptr_dev = (struct device *)kmalloc(sizeof(struct device), GFP_KERNEL);
    submit_struct_ptr(ptr_dev);

	return 0;
}

void cleanup_module(void)
{
    checker_kfree(ptr);
    checker_kfree(ptr_int);
    checker_kfree(ptr_dev);
}

MODULE_LICENSE("GPL");
