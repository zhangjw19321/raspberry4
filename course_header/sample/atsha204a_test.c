#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "atsha204a_i2c.h"
#include "atsha204a_comm.h"
#include "sha204_helper.h"
#include "atsha204a.h"
void printf_hex_array(char* tip, int len, unsigned char* array)
{
    int i;
    printf("\n");
    printf("%s",tip);
    for(i = 0; i < len; i++){
        printf("0x%x  ", array[i]);
    }
    printf("\n");
}

int main(int argc, char** argv)
{
    unsigned char data[32];
    unsigned int word_addr = 0;
    u_int8_t secret_key[32] = {
         0x90,0x23,0x42,0xdd,0xb8,0xa6,0x0d,0x7e,
         0x83,0xb1,0x42,0xd1,0x61,0xbf,0xfa,0x1a,
         0x97,0x3a,0x52,0xac,0xb9,0x20,0x6f,0x99,
         0xb4,0x17,0xf7,0x8a,0x82,0x8e,0x8c,0x9f
    };
    memset(data, 154, 32);
    atsha204a_set_i2c_func(atsha_i2c_read, atsha_i2c_write);
    atsha204a_read_sn(data);
    printf_hex_array("sn: ", 9, data);

    if (data[0] == 0x01 && data[1] == 0x23 && data[8] == 0xee){
        printf("Success\n");
    }
    else{
        printf("Failed\n");
    }
    printf("****************");
    u_int8_t num_in[20] = {0x00};
    int i;
    srand((unsigned)time(NULL));
    for(i = 0; i < 20; i++){
        
        int x = (rand() % (100-1)) + 1;
        printf("x is %d \n", x);
        num_in[i] = x;
    }
    char nonce_response[32]  = {};
	char mac_response[32]    = {};
	u_int8_t soft_digest [32];
    struct sha204h_nonce_in_out nonce_param;
	struct sha204h_mac_in_out mac_param;
	struct sha204h_temp_key tempkey;
    // step one --- run nonce
    if(atsha204a_run_nonce(num_in,nonce_response)==SHA204_CMD_FAIL){
    printf("run nonce failed \n");
    return -1;    
    }
    printf_hex_array("nonce response is: ", 32, nonce_response);
    // step two -- simulate nonce
    nonce_param.mode = NONCE_MODE_NO_SEED_UPDATE;
	nonce_param.num_in = num_in;	
	nonce_param.rand_out = nonce_response;	
	nonce_param.temp_key = &tempkey;
    if (sha204h_nonce(&nonce_param) == SHA204_FAIL){
		printf("simulate nonce fail \n ");
		return SHA204_FAIL;
	}
    
    // step three run mac
    if(atsha204a_run_mac(mac_response)==SHA204_CMD_FAIL){
    printf("run mac failed \n");
    return -1;    
    }
    printf_hex_array("mac response is: ", 32, mac_response);
    // step four -- simulate mac
    mac_param.mode = MAC_MODE_BLOCK2_TEMPKEY;
	mac_param.key_id = 0;
	mac_param.challenge = NULL;
	mac_param.key = secret_key;
	mac_param.otp = NULL;
	mac_param.sn = NULL;
	mac_param.response = soft_digest;
	mac_param.temp_key = &tempkey;
	if (sha204h_mac(&mac_param) == SHA204_FAIL){
		printf("simulate mac fail \n ");
		return SHA204_FAIL;
	}
    // step five -- 
    printf_hex_array("simulate soft digest is: ", 32, soft_digest);
    printf_hex_array("mac return serial is: ", 32, mac_response);
    if(memcmp(soft_digest,mac_response,32) == 0){
        printf("Authentication: success \n ");
    }else{
        printf("Authentication: fail \n ");
    } 
} 


