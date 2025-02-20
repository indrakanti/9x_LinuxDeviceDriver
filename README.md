# 9x
# Let's Learn Linux Device Drivers and Applications!

Welcome to this project! This repository is designed to help you learn about **Linux Device Drivers (LDD)** and **Linux Applications** by implementing multiple kernel drivers and a user-space test application.

## **About This Project**
This project provides:
- A **step-by-step approach** to understanding Linux Device Drivers.
- Multiple **synchronization mechanisms** used in driver development.
- A **user-space application** to interact with kernel modules.

![9X Logo](9xLogo.webp)

## **What You Will Learn**
- Writing **character device drivers** in Linux.
- Implementing different **locking mechanisms** (Mutex, Spinlock, Semaphore).
- **Building** and **interacting** with kernel modules.
- Developing a **user-space application** to communicate with drivers.
- Using **system calls like `read()`, `write()`, `open()`, and `ioctl()`**.

## **Getting Started**
### **1. Clone the Repository**
```bash
git clone <repo-url>
cd <repo-directory>
```

### **2. Build Everything (User-Space App + Kernel Drivers)**
```bash
make
```

### **3. Load the Kernel Modules**
```bash
sudo insmod 9x_abcd.ko  # Replace with 9x_hpspinlock.ko or 9x_reentrant.ko
```

### **4. Run the Test Application**
```bash
./9x_app
```

### **5. Check Kernel Logs**
```bash
dmesg | tail -n 50
```

## **Contribute & Explore**
This project is open to contributions! Feel free to experiment, modify, and enhance the drivers and applications.

## **License**
This project is released under the **GPL v2** license.

🚀 **Happy Learning!** 🚀
