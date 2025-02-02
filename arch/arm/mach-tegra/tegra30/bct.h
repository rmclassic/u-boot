#ifndef BCT_H
#define BCT_H
#include "uboot_aes.h"
#include <common.h>


#define BCT_ERR_SUCCESS       0
#define BCT_ERR_INVALID_SIZE  1
#define BCT_KEY_SIZE          128

int decrypt_bct(u8 *bct_image, u32 bct_size, u8 *key, u8* bct_dec);
int encrypt_bct(u8 *bct_image, u32 bct_size, u8 *key, u8* bct_enc);
#endif
