// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright 2020 Compass Electronics Group, LLC
 */

#include <common.h>
#include <asm/global_data.h>
#include <asm/io.h>
#include <asm/arch/rcar-mstp.h>
#include <linux/delay.h>
#include <env.h>

DECLARE_GLOBAL_DATA_PTR;

/* HSUSB block registers */
#define HSUSB_REG_LPSTS			0xE6590102
#define HSUSB_REG_LPSTS_SUSPM_NORMAL	BIT(14)
#define HSUSB_REG_UGCTRL2		0xE6590184
#define HSUSB_REG_UGCTRL2_USB0SEL	0x30
#define HSUSB_REG_UGCTRL2_USB0SEL_EHCI	0x10
#define HSUSB_REG_UGCTRL2_RESERVED_3	0x00000001 /* bit[3:0] must be B'0001 */
#define HSUSB_MSTP704		BIT(4)	/* HSUSB */

static u8 get_SoC_letter(void)
{
	const u8* name = rzg_get_cpu_name();

	if (*name)
		return name[6];

	return 0;
}

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

int board_late_init(void)
{
	/* If already defined, exit */
	if (!env_get("fdt_file"))
	{
		switch (get_SoC_letter())
		{
		case 'A':
			env_set("fdt_file", "r8a774a1-beacon-rzg2m-kit.dtb");
			break;
		case 'B':
			env_set("fdt_file", "r8a774b1-beacon-rzg2n-kit.dtb");
			break;
		case 'E':
			env_set("fdt_file", "r8a774e1-beacon-rzg2h-kit.dtb");
			break;
		}
	}

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

#if IS_ENABLED(CONFIG_MULTI_DTB_FIT)
int board_fit_config_name_match(const char *name)
{
	switch (get_SoC_letter())
	{
	case 'A':
		return strcmp(name, "r8a774a1-beacon-rzg2m-kit") ? -1 : 0;
	case 'B':
		return strcmp(name, "r8a774b1-beacon-rzg2n-kit") ? -1 : 0;
	case 'E':
		return strcmp(name, "r8a774e1-beacon-rzg2h-kit") ? -1 : 0;
	}
	return -1;
}
#endif
