#include <common.h>
#include <log.h>
#include <linux/errno.h>
#include "uboot_aes.h"
#include "bct.h"
#include "crypto.h"
#include "bct_t30.c"

int bct_update_bl(u8 *bct_image, u32 bct_size, u8 *bl, u32 bl_size, u8* sbk)
{
  u8 bl_hash[AES128_KEY_LENGTH] = {0};

  if (BCT_ERR_SUCCESS != decrypt_bct(bct_image, bct_size, sbk, bct_image)) //in-place decryption
  {
    return -1;
  }

  nvboot_config_table boot_tbl = {0};
  if (0 != if_bct_is_t30_get_soc_config(bct_image, &boot_tbl))
  {
    return -1;
  }

  sign_data_block(bl, bl_size, bl_hash, sbk);

  memcpy((u8*)&boot_tbl.bootloader[0].crypto_hash, bl_hash, NVBOOT_CMAC_AES_HASH_LENGTH * 4);
  boot_tbl.bootloader[0].entry_point = 0x82000000;

  encrypt_bct((u8*)&boot_tbl, bct_size, sbk, bct_image);
  return BCT_ERR_SUCCESS;
}

int decrypt_bct(u8 *bct_image, u32 bct_size, u8 *key, u8 *bct_dec)
{
  u8 expanded_key[AES128_EXPAND_KEY_LENGTH];
  u8 iv[16] = { 0 };

  if (bct_size % AES_BLOCK_LENGTH != 0)
    return BCT_ERR_INVALID_SIZE;

  aes_expand_key(key, BCT_KEY_SIZE, expanded_key);
  aes_cbc_decrypt_blocks(BCT_KEY_SIZE, expanded_key, iv, (u8*)bct_image, bct_dec, bct_size / AES_BLOCK_LENGTH);
  return BCT_ERR_SUCCESS;
}


int encrypt_bct(u8 *bct_image, u32 bct_size, u8 *key, u8 *bct_enc)
{
  u8 expanded_key[AES128_EXPAND_KEY_LENGTH];
  u8 iv[16] = { 0 };

  if (bct_size % AES_BLOCK_LENGTH != 0)
    return BCT_ERR_INVALID_SIZE;

  aes_expand_key(key, BCT_KEY_SIZE, expanded_key);
  aes_cbc_encrypt_blocks(BCT_KEY_SIZE, expanded_key, iv, (u8*)bct_image, bct_enc, bct_size / AES_BLOCK_LENGTH);
  return BCT_ERR_SUCCESS;
}
