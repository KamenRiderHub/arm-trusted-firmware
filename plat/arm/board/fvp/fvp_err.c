/*
 * Copyright (c) 2019-2021, Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <errno.h>

#include <common/debug.h>
//【改】增加cx_wdt.h头文件引用
#include <drivers/arm/cx_wdt.h>

#include <drivers/arm/sp805.h>
#include <drivers/cfi/v2m_flash.h>
#include <lib/mmio.h>
#include <plat/arm/common/plat_arm.h>
#include <platform_def.h>
//【改】增加cx_platform_def.h头文件引用
#include <cx_platform_def.h>
/*
 * FVP error handler
 */
__dead2 void plat_arm_error_handler(int err)
{
	/* Propagate the err code in the NV-flags register */
	//【改】没有VE System Registers，暂时能输出看到错误信息
	//mmio_write_32(V2M_SYS_NVFLAGS_ADDR, (uint32_t)err);

	console_flush();

	/* Setup the watchdog to reset the system as soon as possible */
	//【改】改成使用我们自己的wdt的refresh，参数也选择来自cx_platform_def.h
	//sp805_refresh(ARM_SP805_TWDG_BASE, 1U);
	cx_wdt_refresh(CX_WDT_BASE, 1U, 1U);

	for (;;)
		wfi();
}

void __dead2 plat_arm_system_reset(void)
{
	/* Write the System Configuration Control Register */
	mmio_write_32(V2M_SYSREGS_BASE + V2M_SYS_CFGCTRL,
		      V2M_CFGCTRL_START |
		      V2M_CFGCTRL_RW |
		      V2M_CFGCTRL_FUNC(V2M_FUNC_REBOOT));
	wfi();
	ERROR("FVP System Reset: operation not handled.\n");
	panic();
}
