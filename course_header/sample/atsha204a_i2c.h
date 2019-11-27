#ifndef ATSHA204A_I2C_H
#define ATSHA204A_I2C_H

unsigned char atsha_i2c_write(unsigned char addr1, unsigned char addr2, unsigned char *data, int len);
unsigned char atsha_i2c_read(unsigned char addr1, unsigned char addr2, unsigned char *data, int len);

#endif

