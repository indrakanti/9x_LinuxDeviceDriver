#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

#define DEVICE_PATH "/dev/9x_cdrv"

int main() {
    int fd, i;
    char my_buf[4000];
    ssize_t bytes_read, bytes_written;

    /* Fill buffer with '*' */
    memset(my_buf, '*', sizeof(my_buf));

    /* Open the device with read/write permissions */
    printf("[%d] - Opening device %s\n", getpid(), DEVICE_PATH);
    fd = open(DEVICE_PATH, O_RDWR | O_SYNC);
    if (fd < 0) {
        perror("Device could not be opened");
        return 1;
    }
    printf("Device opened with ID [%d]\n", fd);

    /* Write the contents of my buffer into the device */
    bytes_written = write(fd, my_buf, sizeof(my_buf));
    if (bytes_written < 0) {
        perror("Write error");
        close(fd);
        return 1;
    }
    printf("Wrote %zd bytes to the device\n", bytes_written);

    /* Clear buffer before reading */
    memset(my_buf, 0, sizeof(my_buf));

    /* Read 70 characters from offset 20 */
    lseek(fd, 20, SEEK_SET);
    bytes_read = read(fd, my_buf, 70);
    if (bytes_read < 0) {
        perror("Read error");
        close(fd);
        return 1;
    }
    printf("Read %zd bytes from the device:\n%s\n", bytes_read, my_buf);

    /* Clear buffer again */
    memset(my_buf, 0, sizeof(my_buf));

    /* Write a new string into the device */
    bytes_written = write(fd, "Veda Solutions", 14);
    if (bytes_written < 0) {
        perror("Write error");
        close(fd);
        return 1;
    }
    printf("Wrote %zd bytes to the device\n", bytes_written);

    /* Move to offset 60 and read 70 bytes */
    lseek(fd, 60, SEEK_SET);
    bytes_read = read(fd, my_buf, 70);
    if (bytes_read < 0) {
        perror("Read error");
        close(fd);
        return 1;
    }
    printf("Read %zd bytes from the device:\n%s\n", bytes_read, my_buf);

    /* Close the device */
    close(fd);
    printf("Device closed successfully.\n");

    return 0;
}
