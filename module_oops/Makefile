obj-m   := solution.o 
KDIR    := /lib/modules/$(shell uname -r)/build
PWD     := $(shell pwd) 
SYM=$(PWD) 
#ccflags-y =-g -DDEBUG
 
all: 
	$(MAKE) -C $(KDIR) SUBDIRS=$(PWD) modules
