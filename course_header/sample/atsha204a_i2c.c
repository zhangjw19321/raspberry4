#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include "atsha204a_i2c.h"

// i2c driver related, change by yourself
#define I2C_SLAVE_DEV   "/dev/i2c-1"
#define I2C_SLAVE_FORCE	0x0706
void printf_hex(char* tip, int len, unsigned char* array)
{
    int i;
    printf("\n");
    printf("%s",tip);
    for(i = 0; i < len; i++){
        printf("0x%x  ", array[i]);
    }
    printf("\n");
}




unsigned char atsha_i2c_write(unsigned char dev_addr, unsigned char sub_addr, unsigned char *data, int len)
{
    int fd;
    int ret;
    unsigned char buf[64];

    fd = open(I2C_SLAVE_DEV, O_RDWR);
    if (fd < 0)
    {
        printf("open i2c dev error!\n");
        return -1;
    }

    ret = ioctl(fd, I2C_SLAVE_FORCE, dev_addr>>1);
    if (ret < 0)
    {
        printf ("set dev error!\n");
        close(fd);
        return ret;
    }

    //reset to awake atsha204a
    buf[0] = 0x00;
    write(fd, buf, 1);
    usleep(5000);

    buf[0] = sub_addr;
    memcpy(&buf[1], data, len);

    ret = write(fd, buf, len+1);
    printf_hex("write data is:in i2c write: \n", 32,data);
    if (ret < 0)
    {
        printf("I2C write error!\n");
        close(fd);
        return -1;
    }
    close(fd);
    usleep(1000);
    return 0;
}

unsigned char atsha_i2c_read(unsigned char dev_addr, unsigned char sub_addr, unsigned char *data, int len)
{
    int fd;
    int ret;
    unsigned char buf[64] = {0};

    fd = open(I2C_SLAVE_DEV, O_RDWR);
    if (fd < 0)
    {
        printf("open i2c dev error!\n");
        return -1;
    }

    ret = ioctl(fd, I2C_SLAVE_FORCE, dev_addr>>1);
    if (ret < 0)
    {
        printf ("set dev error!\n");
        close(fd);
        return ret;
    }

    buf[0] = sub_addr;
    ret = read(fd, buf, len);
    if (ret < 0)
    {
        printf(" I2C read error!\n");
        close(fd);
        return -1;
    }
    memcpy(data, buf, len);
	printf_hex("read data is:in i2c read: \n", 32,data);
    close(fd);
    usleep(1000);
    return 0;
}

