/*
 * Copyright (c) 2013-2021, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>
#include <errno.h>

#include <bl1/bl1.h>
#include <common/tbbr/tbbr_img_def.h>
#include <drivers/arm/smmu_v3.h>
#include <drivers/arm/sp805.h>

//【改】增加cx_wdt.h头文件引用
#include <drivers/arm/cx_wdt.h>

#include <lib/mmio.h>
#include <plat/arm/common/arm_config.h>
#include <plat/arm/common/plat_arm.h>
#include <plat/arm/common/arm_def.h>
#include <plat/common/platform.h>
#include "fvp_private.h"

//【改】增加cx_platform_def.h头文件引用
#include <cx_platform_def.h>

//【改】去掉原先加入的#include "cx_console.h"
//#include "cx_console.h"

//【改】注释掉这里原本的修改方案，因为bl1_tzram_layout并没有在此文件定义
// //这个arm_bl1_early_platform_setup是覆写掉原来弱定义的函数
// void arm_bl1_early_platform_setup(void)
// {
// 	#if !ARM_DISABLE_TRUSTED_WDOG
// 		/* Enable watchdog */
// 		plat_arm_secure_wdt_start();
// 	#endif

// 		/* Initialize the CX console to provide early debug support */
// 		cx_console_boot_init();

// 		/* Allow BL1 to see the whole Trusted RAM */
// 		bl1_tzram_layout.total_base = ARM_BL_RAM_BASE;
// 		bl1_tzram_layout.total_size = ARM_BL_RAM_SIZE;

// 	#if TRANSFER_LIST
// 		secure_tl = transfer_list_init((void *)PLAT_ARM_EL3_FW_HANDOFF_BASE,
// 						PLAT_ARM_FW_HANDOFF_SIZE);
// 		assert(secure_tl != NULL);
// 	#endif
// }
/*******************************************************************************
 * Perform any BL1 specific platform actions.
 ******************************************************************************/
void bl1_early_platform_setup(void)
{
	// ATF_BP(" START: arm_bl1_early_platform_setup in bl1_early_platform_setup [fvp_bl1_setup]\n");
	
	arm_bl1_early_platform_setup();

	ATF_BP(" FINISH: arm_bl1_early_platform_setup in bl1_early_platform_setup [fvp_bl1_setup]\n");


	/* Initialize the platform config for future decision making */
	fvp_config_setup();

	//【改】注释掉fvp_interconnect_init和fvp_interconnect_enable，不管理interconnect相关内容（cmn）
	/*
	 * Initialize Interconnect for this cluster during cold boot.
	 * No need for locks as no other CPU is active.
	 */
	//fvp_interconnect_init();
	/*
	 * Enable coherency in Interconnect for the primary CPU's cluster.
	 */
	//fvp_interconnect_enable();
}

void plat_arm_secure_wdt_start(void)
{
	//【改】改成使用我们自己的wdt的start，参数也选择来自cx_platform_def.h
	//sp805_start(ARM_SP805_TWDG_BASE, ARM_TWDG_LOAD_VAL);
	cx_wdt_start(CX_WDT_BASE, CX_TWDG_CLK_HZ, CX_TWDG_TIMEOUT_SEC);
}

void plat_arm_secure_wdt_stop(void)
{
	//【改】改成使用我们自己的wdt的stop，参数也选择来自cx_platform_def.h
	//sp805_stop(ARM_SP805_TWDG_BASE);
	cx_wdt_stop(CX_WDT_BASE);
}

void bl1_platform_setup(void)
{
	arm_bl1_platform_setup();

	/* Initialize System level generic or SP804 timer */
	//【改】timer被scp管，我们atf暂不需要设置
	fvp_timer_init();

	/* On FVP RevC, initialize SMMUv3 */
	ATF_BP(" START: smmuv3_security_init(PLAT_FVP_SMMUV3_BASE) in bl1_platform_setup [fvp_bl1_setup]\n");

	
	if ((arm_config.flags & ARM_CONFIG_FVP_HAS_SMMUV3) != 0U)
		smmuv3_security_init(PLAT_FVP_SMMUV3_BASE);

	ATF_BP(" FINISH: smmuv3_security_init(PLAT_FVP_SMMUV3_BASE) in bl1_platform_setup [fvp_bl1_setup]\n");

}

__dead2 void bl1_plat_fwu_done(void *client_cookie, void *reserved)
{
	uint32_t nv_flags = mmio_read_32(V2M_SYS_NVFLAGS_ADDR);

	/* Clear the NV flags register. */
	mmio_write_32((V2M_SYSREGS_BASE + V2M_SYS_NVFLAGSCLR),
		      nv_flags);

	/* Setup the watchdog to reset the system as soon as possible */
	//【改】改成使用我们自己的wdt的refresh，参数也选择来自cx_platform_def.h
	//sp805_refresh(ARM_SP805_TWDG_BASE, 1U);
	cx_wdt_refresh(CX_WDT_BASE, 1U, 1U);

	while (true)
		wfi();
}

/*******************************************************************************
 * The following function checks if Firmware update is needed by checking error
 * reported in NV flag.
 ******************************************************************************/
bool plat_arm_bl1_fwu_needed(void)
{
	// int32_t nv_flags = (int32_t)mmio_read_32(V2M_SYS_NVFLAGS_ADDR);

	// /* if image load/authentication failed */
	// return ((nv_flags == -EAUTH) || (nv_flags == -ENOENT));

	// 【改】自己的芯片暂时不需要BL1u
	return false;
}
