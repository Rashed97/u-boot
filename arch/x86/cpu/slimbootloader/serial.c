// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2019 Intel Corporation <www.intel.com>
 */

#include <common.h>
#include <dm.h>
#include <ns16550.h>
#include <serial.h>
#include <asm/arch/slimbootloader.h>

/**
 * The serial port info hob is generated by Slim Bootloader, so eligible for
 * Slim Bootloader based boards only.
 */
static int slimbootloader_serial_ofdata_to_platdata(struct udevice *dev)
{
	const efi_guid_t guid = SBL_SERIAL_PORT_INFO_GUID;
	struct sbl_serial_port_info *data;
	struct ns16550_platdata *plat = dev->platdata;

	if (!gd->arch.hob_list)
		panic("hob list not found!");

	data = hob_get_guid_hob_data(gd->arch.hob_list, NULL, &guid);
	if (!data) {
		debug("failed to get serial port information\n");
		return -ENOENT;
	}
	debug("type:%d base=0x%08x baudrate=%d stride=%d clk=%d\n",
	      data->type,
	      data->base,
	      data->baud,
	      data->stride,
	      data->clk);

	/*
	 * The data->type provides port io or mmio access type info,
	 * but the access type will be controlled by
	 * CONFIG_SYS_NS16550_PORT_MAPPED or CONFIG_SYS_NS16550_MEM32.
	 *
	 * TBD: ns16550 access type configuration in runtime.
	 *      ex) plat->access_type = data->type
	 */
	plat->base = data->base;
	/* ns16550 uses reg_shift, then covert stride to shift */
	plat->reg_shift = data->stride >> 1;
	plat->clock = data->clk;

	return 0;
}

static const struct udevice_id slimbootloader_serial_ids[] = {
	{ .compatible = "intel,slimbootloader-uart" },
	{}
};

U_BOOT_DRIVER(serial_slimbootloader) = {
	.name	= "serial_slimbootloader",
	.id	= UCLASS_SERIAL,
	.of_match = slimbootloader_serial_ids,
	.ofdata_to_platdata = slimbootloader_serial_ofdata_to_platdata,
	.platdata_auto_alloc_size = sizeof(struct ns16550_platdata),
	.priv_auto_alloc_size = sizeof(struct NS16550),
	.probe	= ns16550_serial_probe,
	.ops	= &ns16550_serial_ops,
};