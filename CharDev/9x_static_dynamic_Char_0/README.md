# Character Device Driver (9x_cdrv)

## Overview
This project implements two versions of a simple character device driver for Linux: one with a dynamic major number allocation and another with a fixed major number.

## Features
- Supports basic read and write operations.
- Two driver implementations:
  - **Dynamic Version**: Uses `alloc_chrdev_region()` for major number allocation and `kmalloc()` for buffer allocation.
  - **Static Version**: Uses a fixed major number and a statically allocated buffer.
- Registers and unregisters the character device dynamically using `alloc_chrdev_region()`.
- Uses a dynamically allocated buffer (`kmalloc`) to store written data.

## Explanation
The three C code files represent different parts of the character device driver implementation with distinct approaches:

1. **9x_cdrv_dynamic.c (Character Driver Dynamic)**
   - Dynamically allocates the character device major number using `alloc_chrdev_region()`.
   - Uses `kmalloc()` to allocate memory for the device buffer dynamically.
   - The driver registers and unregisters dynamically during initialization and cleanup.
   - More flexible as it does not rely on a fixed major number.

2. **9x_cdrv_static.c (Skeleton Driver)**
   - Uses a fixed major number (`#define MAJORNO 300`).
   - Does not dynamically allocate the major number.
   - The device buffer is statically allocated as a global array (`static char device_buffer[MAX_LENGTH]`).
   - This is a simpler and more basic driver, useful for learning or testing.
   - If another device is already using major 300, it can cause conflicts.

3. **test_skel.c (Test Program)**
   - A user-space program that interacts with the driver.
   - Opens the device file (`/dev/9x_cdrv`), writes data, and attempts to read it.
   - Helps verify if the kernel module is functioning correctly.

### Key Differences Summary
| Feature | chr_drv_skel.c | chr_drv_dynamic.c | test_skel.c |
|---------|---------------|-------------------|-------------|
| **Major Number** | Fixed (300) | Dynamically allocated | Uses `/dev/9x_cdrv` |
| **Memory Allocation** | Static (`char device_buffer[MAX_LENGTH]`) | Dynamic (`kmalloc()`) | N/A (User-space) |
| **Flexibility** | Simpler but fixed | More dynamic, avoids conflicts | User-space interaction |
| **Purpose** | Basic driver skeleton | Advanced driver implementation | Testing interface |

## Installation

### Building the Drivers
1. Ensure you have the Linux kernel headers installed.
2. Run the following command to build both modules:
   ```sh
   make
   ```

### Loading the Drivers
To insert the dynamic module into the kernel, run:
```sh
sudo insmod 9x_cdrv_dynamic.ko
```

To insert the static module into the kernel, run:
```sh
sudo insmod 9x_cdrv_static.ko
```

### Unloading the Drivers
To remove the dynamic module from the kernel, run:
```sh
sudo rmmod 9x_cdrv_dynamic
```

To remove the static module from the kernel, run:
```sh
sudo rmmod 9x_cdrv_static
```

## Testing
1. Create a device node:
   ```sh
   sudo mknod /dev/9x_cdrv c $(grep 9x_cdrv /proc/devices | awk '{print $1}') 0
   sudo chmod 666 /dev/9x_cdrv
   ```
2. Compile and run the test program:
   ```sh
   gcc -o 9x_testchar 9x_testchar.c
   ./9x_testchar
   ```

## Author
**Ram Indrakanti**

## License
This project is licensed under the **MIT License**.
Honestly, I am not worried.. can be used for learning or as a skeleton for your needs.

