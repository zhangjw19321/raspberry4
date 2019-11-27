#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <time.h>
#include <sys/time.h>
#include "atsha204a_comm.h"
#include "sha204_helper.h"
#include "sha204_comm_marshaling.h"
#include "atsha204a.h"
#define ATSHA204_ADDR (0xc8)

static unsigned char (*atsha204a_i2c_read)(unsigned char, unsigned char, unsigned char *, int);
static unsigned char (*atsha204a_i2c_write)(unsigned char, unsigned char, unsigned char *, int);

void printf_h(char* tip, int len, unsigned char* array)
{
    int i;
    printf("\n");
    printf("%s",tip);
    for(i = 0; i < len; i++){
        printf("0x%x  ", array[i]);
    }
    printf("\n");
}



int atsha204a_set_i2c_func(
    unsigned char (*i2c_read)(unsigned char, unsigned char, unsigned char *, int),
    unsigned char (*i2c_write)(unsigned char, unsigned char, unsigned char *, int)
)
{
    if (i2c_read == NULL || i2c_write == NULL) {
        return 1;
    }

    atsha204a_i2c_read = i2c_read;
    atsha204a_i2c_write = i2c_write;
    return 0;
}

static void msleep(int msec)
{
    usleep(msec * 1000);
}

/*
 * CRC计算，从atsha204代码库移植过来
 */
 // from other 

uint8_t *sha204h_include_data(struct sha204h_include_data_in_out *param)
{
	if (param->mode & MAC_MODE_INCLUDE_OTP_88) {
		memcpy(param->p_temp, param->otp, SHA204_OTP_SIZE_8 + SHA204_OTP_SIZE_3);            // use OTP[0:10], Mode:5 is overridden
		param->p_temp += SHA204_OTP_SIZE_8 + SHA204_OTP_SIZE_3;
	}
	else {
		if (param->mode & MAC_MODE_INCLUDE_OTP_64)
			memcpy(param->p_temp, param->otp, SHA204_OTP_SIZE_8);        // use 8 bytes OTP[0:7] for (6)
		else
			memset(param->p_temp, 0, SHA204_OTP_SIZE_8);                 // use 8 zeros for (6)
		param->p_temp += SHA204_OTP_SIZE_8;

		memset(param->p_temp, 0, SHA204_OTP_SIZE_3);                     // use 3 zeros for (7)
		param->p_temp += SHA204_OTP_SIZE_3;
	}

	// (8) 1 byte SN[8] = 0xEE
	*param->p_temp++ = SHA204_SN_8;

	// (9) 4 bytes SN[4:7] or zeros
	if (param->mode & MAC_MODE_INCLUDE_SN)
		memcpy(param->p_temp, &param->sn[4], SHA204_SN_SIZE_4);          //use SN[4:7] for (9)
	else
		memset(param->p_temp, 0, SHA204_SN_SIZE_4);                      //use zeros for (9)
	param->p_temp += SHA204_SN_SIZE_4;

	// (10) 2 bytes SN[0:1] = 0x0123
	*param->p_temp++ = SHA204_SN_0;
	*param->p_temp++ = SHA204_SN_1;

	// (11) 2 bytes SN[2:3] or zeros
	if (param->mode & MAC_MODE_INCLUDE_SN)
		memcpy(param->p_temp, &param->sn[2], SHA204_SN_SIZE_2);          //use SN[2:3] for (11)
	else
		memset(param->p_temp, 0, SHA204_SN_SIZE_2);                      //use zeros for (9)
	param->p_temp += SHA204_SN_SIZE_2;
	
	return param->p_temp;
}



void sha204h_calculate_crc_chain(uint8_t length, uint8_t *data, uint8_t *crc)
{
	uint8_t counter;
	uint16_t crc_register = 0;
	uint16_t polynom = 0x8005;
	uint8_t shift_register;
	uint8_t data_bit, crc_bit;

	crc_register = (((uint16_t) crc[0]) & 0x00FF) | (((uint16_t) crc[1]) << 8);

	for (counter = 0; counter < length; counter++) {
	  for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1) {
		 data_bit = (data[counter] & shift_register) ? 1 : 0;
		 crc_bit = crc_register >> 15;

		 // Shift CRC to the left by 1.
		 crc_register <<= 1;

		 if ((data_bit ^ crc_bit) != 0)
			crc_register ^= polynom;
	  }
	}

	crc[0] = (uint8_t) (crc_register & 0x00FF);
	crc[1] = (uint8_t) (crc_register >> 8);
}


#define rotate_right(value, places) ((value >> places) | (value << (32 - places)))
#define SHA256_BLOCK_SIZE   (64)   // bytes



void sha204h_calculate_sha256(int32_t len, uint8_t *message, uint8_t *digest)
{
	int32_t j, swap_counter, len_mod = len % sizeof(int32_t);
	uint32_t i, w_index;
	int32_t message_index = 0;
	uint32_t padded_len = len + 8; // 8 bytes for bit length
	uint32_t bit_len = len * 8;
	uint32_t s0, s1;
	uint32_t t1, t2;
	uint32_t maj, ch;
	uint32_t word_value;
	uint32_t rotate_register[8];

	union {
		uint32_t w_word[SHA256_BLOCK_SIZE];
		uint8_t w_byte[SHA256_BLOCK_SIZE * sizeof(int32_t)];
	} w_union;

	uint32_t hash[] = {
		0x6a09e667, 0xbb67ae85, 0x3c6ef372,	0xa54ff53a,
		0x510e527f, 0x9b05688c, 0x1f83d9ab, 0x5be0cd19
	};

	const uint32_t k[] = {
		0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
		0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
		0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
		0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
		0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
		0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
		0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
		0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2
	};

	// Process message.
	while (message_index <= (int32_t)padded_len) {

		// Break message into 64-byte blocks.
		w_index = 0;
		do {
			// Copy message chunk of four bytes (size of integer) into compression array.
			if (message_index < (len - len_mod)) {
				for (swap_counter = sizeof(int32_t) - 1; swap_counter >= 0; swap_counter--)
					// No padding needed. Swap four message bytes to chunk array.
					w_union.w_byte[swap_counter + w_index] = message[message_index++];

				w_index += sizeof(int32_t);
			}
			else {
				// We reached last complete word of message {len - (len mod 4)}.
				// Swap remaining bytes if any, append '1' bit and pad remaining
				// bytes of the last word.
				for (swap_counter = sizeof(int32_t) - 1;
						swap_counter >= (int32_t)(sizeof(int32_t) - len_mod); swap_counter--)
					w_union.w_byte[swap_counter + w_index] = message[message_index++];
				w_union.w_byte[swap_counter + w_index] = 0x80;
				for (swap_counter--; swap_counter >= 0; swap_counter--)
					w_union.w_byte[swap_counter + w_index] = 0;

				// Switch to word indexing.
				w_index += sizeof(int32_t);
				w_index /= sizeof(int32_t);

				// Pad last block with zeros to a block length % 56 = 0
				// and pad the four high bytes of "len" since we work only
				// with integers and not with long integers.
				while (w_index < 15)
					 w_union.w_word[w_index++] = 0;
				// Append original message length as 32-bit integer.
				w_union.w_word[w_index] = bit_len;
				// Indicate that the last block is being processed.
				message_index += SHA256_BLOCK_SIZE;
				// We are done with pre-processing last block.
				break;
			}
		} while (message_index % SHA256_BLOCK_SIZE);
		// Created one block.

		w_index = 16;
		while (w_index < SHA256_BLOCK_SIZE) {
			// right rotate for 32-bit variable in C: (value >> places) | (value << 32 - places)
			word_value = w_union.w_word[w_index - 15];
			s0 = rotate_right(word_value, 7) ^ rotate_right(word_value, 18) ^ (word_value >> 3);

			word_value = w_union.w_word[w_index - 2];
			s1 = rotate_right(word_value, 17) ^ rotate_right(word_value, 19) ^ (word_value >> 10);

			w_union.w_word[w_index] = w_union.w_word[w_index - 16] + s0 + w_union.w_word[w_index - 7] + s1;

			w_index++;
		}

		// Initialize hash value for this chunk.
		for (i = 0; i < 8; i++)
			rotate_register[i] = hash[i];

		// hash calculation loop
		for (i = 0; i < SHA256_BLOCK_SIZE; i++) {
			s0 = rotate_right(rotate_register[0], 2)
				^ rotate_right(rotate_register[0], 13)
				^ rotate_right(rotate_register[0], 22);
			maj = (rotate_register[0] & rotate_register[1])
				^ (rotate_register[0] & rotate_register[2])
				^ (rotate_register[1] & rotate_register[2]);
			t2 = s0 + maj;
			s1 = rotate_right(rotate_register[4], 6)
				^ rotate_right(rotate_register[4], 11)
				^ rotate_right(rotate_register[4], 25);
			ch =  (rotate_register[4] & rotate_register[5])
				^ (~rotate_register[4] & rotate_register[6]);
			t1 = rotate_register[7] + s1 + ch + k[i] + w_union.w_word[i];

			rotate_register[7] = rotate_register[6];
			rotate_register[6] = rotate_register[5];
			rotate_register[5] = rotate_register[4];
			rotate_register[4] = rotate_register[3] + t1;
			rotate_register[3] = rotate_register[2];
			rotate_register[2] = rotate_register[1];
			rotate_register[1] = rotate_register[0];
			rotate_register[0] = t1 + t2;
		}

	    // Add the hash of this block to current result.
		for (i = 0; i < 8; i++)
			hash[i] += rotate_register[i];
	}

	// All blocks have been processed.
	// Concatenate the hashes to produce digest, MSB of every hash first.
	for (i = 0; i < 8; i++) {
		for (j = sizeof(int32_t) - 1; j >= 0; j--, hash[i] >>= 8)
			digest[i * sizeof(int32_t) + j] = hash[i] & 0xFF;
	}
}


//original
 
 
 
 
 
 
 
 
static void sha204c_calculate_crc(unsigned char length, unsigned char *data, unsigned char *crc)
{
    unsigned char counter;
    unsigned short crc_register = 0;
    unsigned short polynom = 0x8005;
    unsigned char shift_register;
    unsigned char data_bit, crc_bit;
    for (counter = 0; counter < length; counter++)
    {
        for (shift_register = 0x01; shift_register > 0x00; shift_register <<= 1)
        {
            data_bit = (data[counter] & shift_register) ? 1 : 0;
            crc_bit = crc_register >> 15;
            // Shift CRC to the left by 1.
            crc_register <<= 1;
            if ((data_bit ^ crc_bit) != 0)
            {
                crc_register ^= polynom;
            }
        }
    }
    crc[0] = (unsigned char) (crc_register & 0x00FF);
    crc[1] = (unsigned char) (crc_register >> 8);

}

/*
 * CRC校验，从atsha204代码库移植过来
 */
static unsigned char sha204c_check_crc(unsigned char length, unsigned char  *data)
{
    unsigned char  crc[2];
    sha204c_calculate_crc(length-2, data, crc);
    return (crc[0] == data[length - 2] && crc[1] == data[length - 1]) ? SHA204_SUCCESS : SHA204_BAD_CRC;
}

static int atsha204a_send_command(struct atsha204a_command_packet *command_packet)
{
    unsigned char command_buffer_len = 7 + command_packet->data_len;
    unsigned char command_buffer[48];

    if (command_packet->data_len > 32)
        return SHA204_INVALID_SIZE;

    command_buffer[0] = command_buffer_len;
    command_buffer[1] = command_packet->op_code;
    command_buffer[2] = command_packet->param1;
    command_buffer[3] = command_packet->param2[0];
    command_buffer[4] = command_packet->param2[1];

    if(command_packet->data_len > 0) {
        memcpy(command_buffer+5, command_packet->data, command_packet->data_len);
    }

    sha204c_calculate_crc(command_buffer_len-2, command_buffer, command_buffer+command_buffer_len-2);

    atsha204a_i2c_write(ATSHA204_ADDR, 0x03, command_buffer, command_buffer_len);

    return 0;
}

/*
 * 读取操作命令的response内容，读取的地址都为0x00
 */
static int atsha204a_recv_response(unsigned char length, unsigned char *response_data)
{
    atsha204a_i2c_read(ATSHA204_ADDR, 0x00, response_data, length);
    printf("atsha204a_recv_response:%d",sha204c_check_crc(length,response_data));
    return sha204c_check_crc(length,response_data);
}

int atsha204a_read_sn(unsigned char *sn_data)
{
    struct atsha204a_command_packet get_sn_command;
    unsigned char recv_buffer[35]  = {};

    get_sn_command.op_code   = SHA204_READ;
    get_sn_command.param1    = SHA204_ZONE_COUNT_FLAG | SHA204_ZONE_CONFIG;
    get_sn_command.param2[0] = 0;
    get_sn_command.param2[1] = 0;
    get_sn_command.data_len  = 0;
    get_sn_command.data      = NULL;

    atsha204a_send_command(&get_sn_command);
    msleep(5);
    if(atsha204a_recv_response(35, recv_buffer) == SHA204_SUCCESS){
        memcpy(sn_data , recv_buffer + SHA204_BUFFER_POS_DATA, 4);
        memcpy(sn_data + 4 , recv_buffer + SHA204_BUFFER_POS_DATA + 8, 5);
        return SHA204_SUCCESS;
    }
    return SHA204_CMD_FAIL;
}

int atsha204a_run_nonce(u_int8_t *num_in,unsigned char *nonce_data){
    struct atsha204a_command_packet run_nonce_command;
    unsigned char recv_buffer[35]  = {};
    run_nonce_command.op_code   = 0x16;  
    run_nonce_command.param1    = NONCE_MODE_NO_SEED_UPDATE;
    run_nonce_command.param2[0] = 0;
    run_nonce_command.param2[1] = 0;
    run_nonce_command.data_len  = 20;
    run_nonce_command.data      = num_in;
    atsha204a_send_command(&run_nonce_command);
    msleep(80);
    if(atsha204a_recv_response(35, recv_buffer) == SHA204_SUCCESS){
        memcpy(nonce_data , recv_buffer + SHA204_BUFFER_POS_DATA, 32);
        return SHA204_SUCCESS;
    }
    return SHA204_CMD_FAIL;
}


int atsha204a_run_mac(unsigned char *mac_data){
    struct atsha204a_command_packet run_mac_command;
    unsigned char recv_buffer[35]  = {};
    run_mac_command.op_code   = 0x08;  
    run_mac_command.param1    = MAC_MODE_BLOCK2_TEMPKEY;
    run_mac_command.param2[0] = 0;
    run_mac_command.param2[1] = 0;
    run_mac_command.data_len  = 0;
    run_mac_command.data      = NULL;

    atsha204a_send_command(&run_mac_command);
    msleep(50);
    if(atsha204a_recv_response(35, recv_buffer) == SHA204_SUCCESS){
        memcpy(mac_data , recv_buffer + SHA204_BUFFER_POS_DATA, 32);
        return SHA204_SUCCESS;
    }
    return SHA204_CMD_FAIL;
}



//moshi nonce jisuan


uint8_t sha204h_nonce(struct sha204h_nonce_in_out *param)
{
	uint8_t temporary[SHA204_MSG_SIZE_NONCE];
	uint8_t *p_temp;

	// Check parameters
	if (!param->temp_key || !param->num_in || (param->mode > NONCE_MODE_PASSTHROUGH) || (param->mode == NONCE_MODE_INVALID)
			|| (((param->mode == NONCE_MODE_SEED_UPDATE || (param->mode == NONCE_MODE_NO_SEED_UPDATE)) && !param->rand_out)))
		return -1;

	// Calculate or pass-through the nonce to TempKey->Value
	if ((param->mode == NONCE_MODE_SEED_UPDATE) || (param->mode == NONCE_MODE_NO_SEED_UPDATE)) {
		// Calculate nonce using SHA-256 (refer to data sheet)
		p_temp = temporary;

		memcpy(p_temp, param->rand_out, NONCE_RSP_SIZE_LONG - SHA204_PACKET_OVERHEAD);
		p_temp += NONCE_RSP_SIZE_LONG - SHA204_PACKET_OVERHEAD;

		memcpy(p_temp, param->num_in, NONCE_NUMIN_SIZE);
		p_temp += NONCE_NUMIN_SIZE;

		*p_temp++ = SHA204_NONCE;
		*p_temp++ = param->mode;
		*p_temp++ = 0x00;

		// Calculate SHA256 to get the nonce
		sha204h_calculate_sha256(SHA204_MSG_SIZE_NONCE, temporary, param->temp_key->value);

		// Update TempKey->SourceFlag to 0 (random)
		param->temp_key->source_flag = 0;
	} 
	else if (param->mode == NONCE_MODE_PASSTHROUGH) {
		// Pass-through mode
		memcpy(param->temp_key->value, param->num_in, NONCE_NUMIN_SIZE_PASSTHROUGH);

		// Update TempKey->SourceFlag to 1 (not random)
		param->temp_key->source_flag = 1;
	}

	// Update TempKey fields
	param->temp_key->key_id = 0;
	param->temp_key->gen_data = 0;
	param->temp_key->check_flag = 0;
	param->temp_key->valid = 1;

	return SHA204_SUCCESS;
}

uint8_t sha204h_mac(struct sha204h_mac_in_out *param)
{
	uint8_t temporary[SHA204_MSG_SIZE_MAC];
	uint8_t *p_temp;

////////////////////////////////////////////////////////
//2015-1-16 tony comment	
//	struct sha204h_include_data_in_out include_data = {
//		.otp = param->otp, .sn = param->sn, .mode = param->mode
//	};
	
	struct sha204h_include_data_in_out include_data;
	include_data.otp = param->otp;
	include_data.sn = param->sn;
	include_data.mode = param->mode;
//////////////////////////////////////////////////////	
	// Check parameters
	if (!param->response
		|| (param->mode & ~MAC_MODE_MASK)
		|| (!(param->mode & MAC_MODE_BLOCK1_TEMPKEY) && !param->key)
		|| (!(param->mode & MAC_MODE_BLOCK2_TEMPKEY) && !param->challenge)
		|| ((param->mode & MAC_MODE_USE_TEMPKEY_MASK) && !param->temp_key)
		|| (((param->mode & MAC_MODE_INCLUDE_OTP_64) || (param->mode & MAC_MODE_INCLUDE_OTP_88)) && !param->otp)
		|| ((param->mode & MAC_MODE_INCLUDE_SN) && !param->sn)
		)
		return -1;

	// Check TempKey fields validity if TempKey is used
	if (((param->mode & MAC_MODE_USE_TEMPKEY_MASK) != 0)
			// TempKey.CheckFlag must be 0 and TempKey.Valid must be 1
			&& (param->temp_key->check_flag || (param->temp_key->valid != 1)
			// If either mode parameter bit 0 or bit 1 are set, mode parameter bit 2 must match temp_key.source_flag.
			// Logical not (!) is used to evaluate the expression to TRUE / FALSE first before comparison (!=).
			|| (!(param->mode & MAC_MODE_SOURCE_FLAG_MATCH) != !(param->temp_key->source_flag)))
		)
	{
		// Invalidate TempKey, then return
		param->temp_key->valid = 0;
		return SHA204_CMD_FAIL;
	}

	// Start calculation
	p_temp = temporary;

	// (1) first 32 bytes
	memcpy(p_temp, param->mode & MAC_MODE_BLOCK1_TEMPKEY ? param->temp_key->value : param->key, SHA204_KEY_SIZE);                // use Key[KeyID]
	p_temp += SHA204_KEY_SIZE;

	// (2) second 32 bytes
	memcpy(p_temp, param->mode & MAC_MODE_BLOCK2_TEMPKEY ? param->temp_key->value : param->challenge, SHA204_KEY_SIZE);          // use Key[KeyID]
	p_temp += SHA204_KEY_SIZE;

	// (3) 1 byte opcode
	*p_temp++ = SHA204_MAC;

	// (4) 1 byte mode parameter
	*p_temp++ = param->mode;

	// (5) 2 bytes keyID
	*p_temp++ = param->key_id & 0xFF;
	*p_temp++ = (param->key_id >> 8) & 0xFF;
	
	include_data.p_temp = p_temp;
	sha204h_include_data(&include_data);

	// Calculate SHA256 to get the MAC digest
	sha204h_calculate_sha256(SHA204_MSG_SIZE_MAC, temporary, param->response);

	// Update TempKey fields
	if (param->temp_key)
		param->temp_key->valid = 0;

	return SHA204_SUCCESS;
}







