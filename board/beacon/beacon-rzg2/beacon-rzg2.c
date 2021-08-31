// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2020 Compass Electronics Group, LLC
 */

#include <common.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/arch/rcar-mstp.h>
#include <linux/delay.h>

DECLARE_GLOBAL_DATA_PTR;

/* HSUSB block registers */
#define HSUSB_REG_LPSTS			0xE6590102
#define HSUSB_REG_LPSTS_SUSPM_NORMAL	BIT(14)
#define HSUSB_REG_UGCTRL2		0xE6590184
#define HSUSB_REG_UGCTRL2_USB0SEL	0x30
#define HSUSB_REG_UGCTRL2_USB0SEL_EHCI	0x10
#define HSUSB_REG_UGCTRL2_RESERVED_3	0x00000001 /* bit[3:0] must be B'0001 */
#define HSUSB_MSTP704		BIT(4)	/* HSUSB */

int board_init(void)
{
	/* address of boot parameters */
	gd->bd->bi_boot_params = CONFIG_SYS_TEXT_BASE + 0x50000;

	/* USB1 pull-up */
	setbits_le32(PFC_PUEN6, PUEN_USB1_OVC | PUEN_USB1_PWEN);

	/* Configure the HSUSB block */
	mstp_clrbits_le32(SMSTPCR7, SMSTPCR7, HSUSB_MSTP704);
	udelay(2);
	/* Choice USB0SEL */
	writel(HSUSB_REG_UGCTRL2_USB0SEL_EHCI | HSUSB_REG_UGCTRL2_RESERVED_3,
	       HSUSB_REG_UGCTRL2);
	/* low power status */
	setbits_le16(HSUSB_REG_LPSTS, HSUSB_REG_LPSTS_SUSPM_NORMAL);


	return 0;
}

int dram_init(void)
{
	if (fdtdec_setup_mem_size_base() != 0)
		return -EINVAL;

	return 0;
}

int dram_init_banksize(void)
{
	fdtdec_setup_memory_banksize();

	return 0;
}

#define RST_BASE	0xE6160000
#define RST_CA57RESCNT	(RST_BASE + 0x40)
#define RST_CODE	0xA5A5000F

void reset_cpu(ulong addr)
{
	writel(RST_CODE, RST_CA57RESCNT);
}

#if IS_ENABLED(CONFIG_MULTI_DTB_FIT)
int board_fit_config_name_match(const char *name)
{
	if (!strcmp(rzg_get_cpu_name(), "R8A774A1") && !strcmp(name, "r8a774a1-beacon-rzg2m-kit"))
		return 0;

	if (!strcmp(rzg_get_cpu_name(), "R8A774B1") && !strcmp(name, "r8a774b1-beacon-rzg2n-kit"))
		return 0;

	if (!strcmp(rzg_get_cpu_name(), "R8A774E1") && !strcmp(name, "r8a774e1-beacon-rzg2h-kit"))
		return 0;

	return -1;
}
#endif
