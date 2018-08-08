obj-m   := solution.o 
KDIR    := /lib/modules/$(shell uname -r)/build
PWD     := $(shell pwd) 
SYM=$(PWD) 
ccflags-y =-g -DDEBUG
 
all: 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
clean:
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) clean
load:
	sudo mknod /dev/chrdrv c 240 0
	sudo insmod solution.ko
	sudo dmesg -c

unload:
	sudo rm /dev/chrdrv
	sudo rmmod solution.ko
	sudo dmesg -c
