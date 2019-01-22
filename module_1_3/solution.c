#include <linux/module.h>
#include "checker.h"

extern int array_sum(short *arr, size_t n);
extern ssize_t generate_output(int sum, short *arr, size_t size, char *buf);

char buffer[1024];
long buffer_len = 0;

void solve(long sum, short *arr, size_t n)
{   
    long test_sum = array_sum(arr, n);
    buffer_len = generate_output(test_sum, arr, n, buffer);
    buffer[buffer_len] = 0x00;

    if (sum == test_sum)
        printk(KERN_INFO "%s", buffer);
    else
        printk(KERN_ERR "%s", buffer);
}

int init_module(void)
{
    short task1[7] = {6, 1, 7, 3, 24, 71, 1};
    long task1_sum = 113;
    
    short task2[14] = {61, 12, 3, 3, 24, 12, 1, 612, 21, 12, 5, 61, -6, 1};
    long task2_sum = 822;

    short task3[3] = {612, 21, 12};
    long task3_sum = 645;

    short task4[6] = {61, 12, 3, 24, 12, 1};
    long task4_sum = 113;

    short task5[4] = {5, 61, -6, 1};
    long task5_sum = 61;
    
    short task6[2] = {3, 3};
    long task6_sum = 6;

    short task7[6] = {12, 1, 612, 21, 12, 5};
    long task7_sum = 663;

    short task8[8] = {61, 12, 3, 3, 24, 12, 1, 612};
    long task8_sum = 728;

    short task9[4] = {12, 1, 612, 21};
    long task9_sum = 646;

    short task10[6] = {-6, 2, 3, 8, -9, 12};
    long task10_sum = 10;

    short task11[2] = {26, -5};
    long task11_sum = 21;

    short task12[5] = {64, 2, -6, -2, 2};
    long task12_sum = 60;

	CHECKER_MACRO;

	printk(KERN_INFO "Hello, loading");

    solve(task1_sum, task1, 7);
    solve(task2_sum, task2, 14);
    solve(task3_sum, task3, 3);
    solve(task4_sum, task4, 6);
    solve(task5_sum, task5, 4);
    solve(task6_sum, task6, 2);
    solve(task7_sum, task7, 6);
    solve(task8_sum, task8, 8);
    solve(task9_sum, task9, 4);
    solve(task10_sum, task10, 6);
    solve(task11_sum, task11, 2);
    solve(task12_sum, task12, 5);

	return 0;
}

static void __exit mod_exit(void)
{
    CHECKER_MACRO;
}

module_exit(mod_exit);
MODULE_LICENSE("GPL");
