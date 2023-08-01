// SPDX-License-Identifier: GPL-2.0+
/*
 * Copyright (C) 2018, Bin Meng <bmeng.cn@gmail.com>
 */

#include <common.h>
#include <command.h>
#include <cpu_func.h>
#include <irq_func.h>
#include <linux/delay.h>

__weak void reset_misc(void)
{
}

int do_reset(struct cmd_tbl *cmdtp, int flag, int argc, char *const argv[])
{
	printf("resetting ...\n");

	mdelay(50);				/* wait 50 ms */

	disable_interrupts();

	reset_misc();
	reset_cpu();

	/*NOTREACHED*/
	return 0;
}
