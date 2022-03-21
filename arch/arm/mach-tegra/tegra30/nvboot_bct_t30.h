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
	u32 hash[NVBOOT_CMAC_AES_HASH_LENGTH];
} nvboot_hash;


typedef struct nv_bootloader_info_rec {
	u32 version;
	u32 start_blk;
	u32 start_page;
	u32 length;
	u32 load_addr;
	u32 entry_point;
	u32 attribute;
	nvboot_hash crypto_hash;
} nv_bootloader_info;

typedef struct nvboot_config_table_rec {
	u32 reserved0[0x8];
	u32 boot_data_version;
	u8 reserved1[0xf26];
	u32 bootloader_used;
	nv_bootloader_info bootloader[NVBOOT_MAX_BOOTLOADERS];
	u32 reserved2[0x1fc];
} nvboot_config_table;
#endif /* #ifndef INCLUDED_NVBOOT_BCT_T30_H */
