#ifndef INCLUDED_NVBOOT_BCT_T30_H
#define INCLUDED_NVBOOT_BCT_T30_H

/*
 * Defines the CMAC-AES-128 hash length in 32 bit words. (128 bits = 4 words)
 */
#define NVBOOT_CMAC_AES_HASH_LENGTH 	 4

/**
 * Defines the maximum number of bootloader descriptions in the BCT.
 */
#define NVBOOT_MAX_BOOTLOADERS         4

/**
 * Defines the storage for a hash value (128 bits).
 */
typedef struct nvboot_hash_rec {
	uint32_t hash[NVBOOT_CMAC_AES_HASH_LENGTH];
} nvboot_hash;


typedef struct nv_bootloader_info_rec {
	uint32_t version;
	uint32_t start_blk;
	uint32_t start_page;
	uint32_t length;
	uint32_t load_addr;
	uint32_t entry_point;
	uint32_t attribute;
	nvboot_hash crypto_hash;
} nv_bootloader_info;

typedef struct nvboot_config_table_rec {
	uint32_t reserved0[0x8];
	uint32_t boot_data_version;
	uint8_t reserved1[0xf26];
	uint32_t bootloader_used;
	nv_bootloader_info bootloader[NVBOOT_MAX_BOOTLOADERS];
	uint32_t reserved2[0x1fc];
} nvboot_config_table;
#endif /* #ifndef INCLUDED_NVBOOT_BCT_T30_H */
