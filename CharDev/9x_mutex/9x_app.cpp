/* Character Device Driver Test Application for Multiple Drivers (Read & Write) - C++ Version */

#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <csignal>
#include <unistd.h>
#include <fcntl.h>
#include <cerrno>

#define BUFFER_SIZE 4096

const std::string devices[] = {"/dev/mutex_cdrv", "/dev/reentrant_cdrv", "/dev/spinlock_cdrv"};

void handler(int);

int test_device_rw(const std::string &device_path) {
    int fd;
    ssize_t ret;
    char write_buf[] = "Test data for character device";
    char read_buf[BUFFER_SIZE];
    
    std::cout << "[" << getpid() << "] - Opening device " << device_path << std::endl;
    fd = open(device_path.c_str(), O_RDWR);
    if (fd < 0) {
        perror("Failed to open device");
        return EXIT_FAILURE;
    }
    
    /* Write Operation */
    std::cout << "Writing to device: " << write_buf << std::endl;
    if ((ret = write(fd, write_buf, strlen(write_buf))) < 0) {
        perror("Failed to write to device");
        close(fd);
        return EXIT_FAILURE;
    }
    
    /* Read Operation */
    memset(read_buf, 0, BUFFER_SIZE);
    retry:
    if ((ret = read(fd, read_buf, BUFFER_SIZE)) < 0) {
        if (errno == EINTR) {
            goto retry; /* Interrupted system call, retry read */
        }
        perror("Failed to read from device");
        close(fd);
        return EXIT_FAILURE;
    }
    
    std::cout << "Read from device: " << read_buf << std::endl;
    close(fd);
    return EXIT_SUCCESS;
}

int main() {
    signal(SIGINT, handler);
    for (const auto &device : devices) {
        if (test_device_rw(device) != EXIT_SUCCESS) {
            std::cout << "Test failed for device: " << device << std::endl;
        } else {
            std::cout << "Test successful for device: " << device << std::endl;
        }
    }
    return EXIT_SUCCESS;
}

void handler(int signum) {
    std::cout << "Interrupt signal handled" << std::endl;
}
