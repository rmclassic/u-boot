#include <common.h>
#include <log.h>
#include <linux/errno.h>
#include "nvboot_bct_t30.h"
#define NVBOOT_BOOTDATA_VERSION(a, b) ((((a)&0xffff) << 16) | ((b)&0xffff))

#define BOOTDATA_VERSION_T30		NVBOOT_BOOTDATA_VERSION(0x3, 0x1)
#define BCT_LENGTH 6128

int if_bct_is_t30_get_soc_config(u8 *bct_raw, nvboot_config_table *bct)
{
	memcpy(bct, bct_raw, BCT_LENGTH);

	if (bct->boot_data_version != BOOTDATA_VERSION_T30)
		return 1;

	return 0;
}
