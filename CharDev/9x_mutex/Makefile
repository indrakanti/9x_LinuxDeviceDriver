CC = g++
CFLAGS = -Wall -Wextra -O2
KERNEL_DIR = /lib/modules/$(shell uname -r)/build

# File Names
APP_SRC = 9x_app.cpp
DRIVER_SRC = 9x_hpspinlock.c 9x_mutex.c 9x_reentrant.c
APP_BIN = 9x_app
DRIVER_BIN = 9x_hpspinlock.ko 9x_mutex.ko 9x_reentrant.ko

# Default target
all: app drivers

# Compile user-space application
app: $(APP_SRC)
	$(CC) $(CFLAGS) -o $(APP_BIN) $(APP_SRC)

# Compile kernel modules
drivers:
	make -C $(KERNEL_DIR) M=$(shell pwd) modules

# Clean build files
clean:
	rm -f $(APP_BIN)
	make -C $(KERNEL_DIR) M=$(shell pwd) clean
