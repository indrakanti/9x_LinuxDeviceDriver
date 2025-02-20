# 9x -- Character Device Driver and Test Application

This repository contains:
- A **user-space application (`9x_app.cpp`)** to test multiple character device drivers.
- **Kernel modules** for different synchronization mechanisms:
  - `9x_hpspinlock.c` (High-performance Spinlock-based driver)
  - `9x_mutex.c` (Mutex-based driver for mutual exclusion)
  - `9x_reentrant.c` (Semaphore-based reentrant driver)
- A **Makefile** to build both the application and the kernel modules.

## **Explanation of Drivers**

### **1. High-Performance Spinlock Driver (`9x_hpspinlock.c`)**
- Uses **spinlocks** to ensure exclusive access to the device buffer.
- Suitable for high-performance scenarios where **sleeping is not allowed** (e.g., interrupt context).
- Ensures that **only one process** can access the device at a time, preventing race conditions.
- **Disadvantage**: If misused, it can cause CPU starvation in busy loops.

### **2. Mutex-Based Driver (`9x_mutex.c`)**
- Uses a **mutex (mutual exclusion lock)** to synchronize access.
- Allows only **one process** to access the device at a time but **sleeps** if another process is waiting.
- Suitable for user-space applications that need controlled **blocking behavior**.
- **Safer than spinlocks** in most cases because it avoids busy-waiting.

### **3. Reentrant Semaphore-Based Driver (`9x_reentrant.c`)**
- Uses a **semaphore** to allow multiple readers but exclusive writer access.
- Can be used when multiple processes **can read concurrently**, but writes must be **exclusive**.
- Useful for scenarios where resource-sharing is needed with **controlled access**.
- **Allows recursive locking**, meaning a process can acquire the lock multiple times if needed.

## **How to Use**

### **1. Build Everything (User-Space App + Kernel Drivers)**
```bash
make
```

### **2. Build Only the User-Space Application**
```bash
make app
```

### **3. Build Only the Kernel Drivers**
```bash
make drivers
```

### **4. Clean Build Files**
```bash
make clean
```

## **Loading the Kernel Modules**
Load a driver into the kernel:
```bash
sudo insmod 9x_hpspinlock.ko  # Replace with desired module (9x_mutex.ko or 9x_reentrant.ko)
```

Check if the module is loaded:
```bash
lsmod | grep 9x
```

Unload a driver:
```bash
sudo rmmod 9x_hpspinlock  # Replace with desired module
```

## **Running the User-Space Application**
```bash
./9x_app
```
This will test read and write operations on the following devices:
- `/dev/mutex_cdrv`
- `/dev/reentrant_cdrv`
- `/dev/spinlock_cdrv`

## **Checking Kernel Logs**
Use `dmesg` to check driver output:
```bash
dmesg | tail -n 50
```

## **Contributing**
If you find any issues or improvements, feel free to contribute.

## **License**
This project is released under the **GPL v2** license.
