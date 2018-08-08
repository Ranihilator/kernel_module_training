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
	sudo insmod solution.ko
	sudo dmesg -c

unload:
	sudo rmmod solution.ko
	sudo dmesg -c
