/* atsha204a_ioctl_code.h
 *
 * Copyright (C) 2016 SENSETIME, Inc.
 *
 */
 
#ifndef ATSHA204A_COMM_H
#define ATSHA204A_COMM_H

#define ATSHA204A_ZONE_CONFIG              (  0x00)      //!< Configuration zone
#define ATSHA204A_ZONE_OTP                 (  0x01)      //!< OTP (One Time Programming) zone
#define ATSHA204A_ZONE_DATA                (  0x02)      //!< Data zone
#define ATSHA204A_ZONE_MASK                (  0x03)      //!< Zone mask
#define ATSHA204A_ZONE_COUNT_FLAG          (  0x80)      //!< Zone bit 7 set: Access 32 bytes, otherwise 4 bytes.
#define ATSHA204A_ZONE_ACCESS_4            (     4)      //!< Read or write 4 bytes.
#define ATSHA204A_ZONE_ACCESS_32           (    32)      //!< Read or write 32 bytes.
#define ATSHA204A_ADDRESS_MASK_CONFIG      (0x001F)      //!< Address bits 5 to 7 are 0 for Configuration zone.
#define ATSHA204A_ADDRESS_MASK_OTP         (0x000F)      //!< Address bits 4 to 7 are 0 for OTP zone.
#define ATSHA204A_ADDRESS_MASK             (0x007F)      //!< Address bit 7 to 15 are always 0.

#define ATSHA204A_SN_CMD                     (0x10)       //!< get atsha204a sn 
#define ATSHA204A_NONCE_CMD                  (0x16)       //!< atsha204a run nonce command 
#define ATSHA204A_MAC_CMD                    (0x08)       //!< atsha204a run mac command 
#define ATSHA204A_WRITE_CMD                  (0x12)       //!< atsha204a run writer command 
#define ATSHA204A_READ_CMD                   (0x02)       //!< atsha204a run read command 

#define SHA204_SUCCESS                       (0x01)
#define SHA204_CMD_FAIL                      (0xD3)
#define SHA204_INVALID_SIZE                  (0xE4)
#define SHA204_BAD_CRC                       (0xE5)

#define SHA204_ZONE_CONFIG                   (0x00)
#define SHA204_BUFFER_POS_DATA               (0x01)
#define SHA204_READ                          (0x02)
#define SHA204_ZONE_COUNT_FLAG               (0x80)

#define ATSHA204_ADDR                        (0xc8)

struct atsha204a_command_packet {
    unsigned char  op_code;
    unsigned char  param1;
    unsigned char  param2[2];
    unsigned char  data_len;
    unsigned char  *data;
}atsha204a_command_packet_t;

int atsha204a_set_i2c_func(
    unsigned char (*i2c_read)(unsigned char, unsigned char, unsigned char *, int),
    unsigned char (*i2c_write)(unsigned char, unsigned char, unsigned char *, int)
);
int atsha204a_read_sn(unsigned char *sn_data);
int atsha204a_run_nonce(u_int8_t *num_in,unsigned char *nonce_data);
int atsha204a_run_mac(unsigned char *mac_data);

#endif

