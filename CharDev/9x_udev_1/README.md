# 9x udev

## Overview
This project provides an implementation of a simple character device driver for Linux, designed to interact with user-space programs.

## Features
- Supports fundamental character device operations: **open, read, write, seek, and release**.
- Implements a dynamically allocated buffer for device storage.
- Uses `udev` for automatic device node creation.
- Includes a test program to validate driver functionality.

## File Structure
| File Name            | Description |
|----------------------|-------------|
| **9x_chr_drv_udev.c** | Main character device driver implementation |
| **9x_chr_drv_udev.mod.c** | Auto-generated module metadata file |
| **9x_test_chrdev.c** | User-space test program to interact with the driver |
| **Makefile** | Build script to compile the module and test program |

## Explanation of the Files

### **9x_chr_drv_udev.c** (Character Device Driver)
- Implements core file operations such as:
  - `9xChar_dev_open()` - Opens the device.
  - `9xChar_dev_read()` - Reads data from the device buffer.
  - `9xChar_dev_write()` - Writes data to the device buffer.
  - `9xChar_dev_lseek()` - Allows seeking within the buffer.
  - `9xChar_dev_release()` - Closes the device.
- Uses `alloc_chrdev_region()` to dynamically allocate a major number.
- Manages memory using `kmalloc()` and `kfree()`.

### **9x_chr_drv_udev.mod.c** (Module Metadata)
- **Auto-generated during compilation**.
- Contains versioning, dependencies, and kernel-related metadata.
- Ensures compatibility with the kernel by including symbols like `MODULE_INFO`.
- **Not manually modified**, only needed during the build process.

### **9x_test_chrdev.c** (User-Space Test Program)
- Opens the device file (`/dev/9x_cdrv`).
- Writes and reads data from the device to verify functionality.
- Demonstrates **seek operations** within the device buffer.
- Helps debug the driver behavior by printing read/write results.
- Uses proper error handling and modern APIs for compatibility with newer kernels.

### **Makefile**
- Automates the build process.
- Compiles the kernel module and user-space test program.
- Provides commands to insert and remove the module.
- Includes targets for cleaning up build artifacts and manually creating the device node.

## Installation & Usage

### **Building the Driver and Test Program**
1. Ensure you have the Linux kernel headers installed:
   ```sh
   sudo apt install linux-headers-$(uname -r)
   ```
2. Compile both the driver and test program:
   ```sh
   make
   ```

### **Loading the Driver**
To insert the module into the kernel:
```sh
sudo make load
```

### **Ensuring Device Node Exists and Correct Permissions**
Check if the device node exists and adjust permissions if necessary:
```sh
ls -l /dev/9x_cdrv
sudo chmod 666 /dev/9x_cdrv
```

### **Creating the Device Node (If Needed)**
If `udev` does not create the device node automatically, manually create it:
```sh
sudo make devnode
```

### **Running the Test Program**
Compile and execute the user-space test program:
```sh
make test
./9x_test_chrdev
```

### **Unloading the Driver**
To remove the module from the kernel:
```sh
sudo make unload
```

### **Cleaning Up**
To remove compiled files:
```sh
make clean
```

## Author
**Ram Indrakanti**

## License
This project is licensed under the **MIT License**. It can be freely used for learning or as a reference for developing character device drivers.

