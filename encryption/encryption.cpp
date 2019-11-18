
void mac(){
    // 密钥，加密芯片中也存有一份完全一样的
    uint8_t secret_key[32] = {
         0x11,0x22,0x33,0x44,0x55,0x66,0x77,0x88,
         0x99,0xaa,0x21,0x87,0xf5,0x94,0x6e,0xcd,
         0x0c,0x75,0x5c,0xd5,0x57,0x3c,0x3a,0x40,
         0x9a,0xdf,0xdb,0x83,0x55,0x1b,0xd0,0xd1
    };
    // NONCE 命令中需要的20个随机参数
    uint8_t num_in[20] = {0x00};
    // 软件计算摘要，最终要与加密芯片计算的摘要进行对比
    uint8_t soft_digest [32];
    // IOCTL函数中的参数结构体，包含输入/输出
    struct atsha204a_nonce   nonce_cmd_param;
    struct atsha204a_mac     mac_cmd_param; 
struct sha204h_nonce_in_out   nonce_param;
    struct sha204h_mac_in_out     mac_param; 
    struct sha204h_temp_key       tempkey; 
// 初始化20个随机数： 
int i;
srand((int)time(0));
for(i = 0; i < 20; i++){
    num_in[i] = 1+(uint8_t)(255.0*rand()/(RAND_MAX+1.0));
}
    // 加密验证5个步骤： 
    // 1.加密芯片运行nonce命令，在芯片内部生成tempkey，并返回32Byte的随机数
    nonce_cmd_param.in_mode = NONCE_MODE_NO_SEED_UPDATE; //生成tempkey
    memcpy(nonce_cmd_param.in_numin, num_in, 20);
    if(ioctl(fd, ATSHA204A_NONCE_CMD, &nonce_cmd_param) == 0){
        printf("atsha204a run nonce command fail.\n");
        return ;
    } 
// 2.软件模拟运行NONCE 命令，主要是为了生成tempkey，为后面的计算使用
    nonce_param.mode = NONCE_MODE_NO_SEED_UPDATE;
    nonce_param.num_in = num_in;
    nonce_param.rand_out = nonce_cmd_param.out_response;
    nonce_param.temp_key = &tempkey;
    if (sha204h_nonce(&nonce_param) == 0){
        printf("sha204h_nonce fail \n ");
        return ;
    } 
// 3.加密芯片运行MAC 命令，芯片内部会使用sha256算法生成摘要，并返回结果
    mac_cmd_param.in_mode = MAC_MODE_BLOCK2_TEMPKEY;
    mac_cmd_param.in_slot = 0; //选择第几个slot的密钥进行计算
    mac_cmd_param.in_challenge_len = 0;
    if(ioctl(fd, ATSHA204A_MAC_CMD, &mac_cmd_param) == 0){
        printf("atsha204a run mac command fail.\n");
        return ;
    } 
array_hex_printf("hw mac result : ",32,mac_cmd_param.out_response); 
// 4.软件模拟运行MAC 命令，使用sha256算法生成摘要
    mac_param.mode = MAC_MODE_BLOCK2_TEMPKEY;
    mac_param.key_id = 0;
    mac_param.challenge = NULL;
    mac_param.key = secret_key;
    mac_param.otp = NULL;
    mac_param.sn = NULL;
    mac_param.response = soft_digest;
    mac_param.temp_key = &tempkey;
    if (sha204h_mac(&mac_param) == 0){
        printf("sha204h_mac fail \n ");
        return ;
    } 
array_hex_printf("sw mac result : ",32,soft_digest); 
// 5.加密芯片和软件生成的摘要进行对比，如果相等，则验证成功
    if(memcmp(soft_digest,mac_cmd_param.out_response,32) == 0){
        printf("Authentication: success \n ");
    }else{
        printf("Authentication: fail \n ");
    } 
} 
