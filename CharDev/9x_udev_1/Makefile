# Makefile for compiling 9x Character Device Driver and Test Program

# Kernel build directory (modify if needed)
KDIR := /lib/modules/$(shell uname -r)/build

# Output module name
MODULE_NAME := 9x_chr_drv_udev
TEST_PROGRAM := 9x_test_chrdev

# Source files
obj-m += $(MODULE_NAME).o

# Default target: Compile both driver and test program
all: driver test

# Compile the kernel module
driver:
	$(MAKE) -C $(KDIR) M=$(PWD) modules

# Compile the test program
test:
	gcc -o $(TEST_PROGRAM) $(TEST_PROGRAM).c

# Load the kernel module
load: 
	sudo insmod $(MODULE_NAME).ko
	sudo dmesg | tail -n 20

# Unload the kernel module
unload:
	sudo rmmod $(MODULE_NAME)
	sudo dmesg | tail -n 20

# Clean up generated files
clean:
	$(MAKE) -C $(KDIR) M=$(PWD) clean
	rm -f $(TEST_PROGRAM)
	rm -f *.o *.ko *.mod.c *.mod.o .*.cmd .tmp_versions -r

# Create device node (if udev doesn't automatically create it)
devnode:
	sudo mknod /dev/9x_cdrv c $(shell grep 9x_cdrv /proc/devices | awk '{print $$1}') 0
	sudo chmod 666 /dev/9x_cdrv

.PHONY: all driver test load unload clean devnode
