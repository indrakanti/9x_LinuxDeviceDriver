obj-m := 9x_drv_static.o
obj-m += 9x_drv_dynamic.o

KDIR := /lib/modules/$(shell uname -r)/build
PWD := $(shell pwd)

all:
	$(MAKE) -C $(KDIR) M=$(PWD) modules
	gcc -o 9x_testchar 9x_testchar.c
	$(MAKE) -C $(KDIR) M=$(PWD) modules

clean:
	rm -rf $(wildcard *.o *.ko mo* Mo* *.mod.c *.symvers *.order .*.cmd .tmp_versions)
