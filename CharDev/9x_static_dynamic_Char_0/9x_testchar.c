/* test_char_driver.c
** Author: Ram Indrakanti
** Version: 1.0
*/

#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <errno.h>

int main()
{
    int fd;
    ssize_t ret;
    char test_buffer[12] = "Hello world";
    
    fd = open("/dev/9x_cdrv", O_RDWR);
    if(fd < 0)
    {
        printf("Failed acquiring file descriptor, return status %d\n", fd);
        return -1;
    }
    
    /* Write the contents of test_buffer into the device */
    ret = write(fd, test_buffer, 12);
    ret = read(fd, test_buffer, 3000);
    
    if(ret < 0)
    {
        printf("Read operation failed with return status %d\n", ret);
    }
    
    close(fd);
    return 0;
}
