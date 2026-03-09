/*
 * Copyright (c) 2013-2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <arch_helpers.h>
#include <arch_features.h>
#include <bl1/bl1.h>
#include <bl2/bl2.h>
#include <common/bl_common.h>
#include <common/build_message.h>
#include <common/debug.h>
#include <drivers/auth/auth_mod.h>
#include <drivers/auth/crypto_mod.h>
#include <drivers/console.h>
#include <drivers/fwu/fwu.h>
#include <lib/bootmarker_capture.h>
#include <lib/extensions/pauth.h>
#include <lib/pmf/pmf.h>
#include <plat/common/platform.h>

#include "bl2_private.h"
#include "plat/arm/board/cx/cx_uart.h"
#include "plat/arm/board/cx/cx_console.h"

#ifdef __aarch64__
#define NEXT_IMAGE	"BL31"
#else
#define NEXT_IMAGE	"BL32"
#endif

#if ENABLE_RUNTIME_INSTRUMENTATION
	PMF_REGISTER_SERVICE(bl_svc, PMF_RT_INSTR_SVC_ID,
		BL_TOTAL_IDS, PMF_DUMP_ENABLE);
#endif

#if RESET_TO_BL2
/*******************************************************************************
 * Setup function for BL2 when RESET_TO_BL2=1
 ******************************************************************************/
void bl2_el3_setup(u_register_t arg0, u_register_t arg1, u_register_t arg2,
		   u_register_t arg3)
{
	ATF_BP(" Here is bl2_el3_setup [bl2_main]\n");
	ATF_BP(" START: plat_setup_early_console(); in bl2_el3_setup [bl2_main]\n");

	/* Enable early console if EARLY_CONSOLE flag is enabled */
	plat_setup_early_console();
	ATF_BP(" FINISH: plat_setup_early_console(); in bl2_el3_setup [bl2_main]\n");

	ATF_BP(" START: bl2_el3_early_platform_setup(arg0, arg1, arg2, arg3); in bl2_el3_setup [bl2_main]\n");
	/* Perform early platform-specific setup */
	bl2_el3_early_platform_setup(arg0, arg1, arg2, arg3);
	ATF_BP(" FINISH: bl2_el3_early_platform_setup(arg0, arg1, arg2, arg3); in bl2_el3_setup [bl2_main]\n");

	ATF_BP(" START: bl2_el3_plat_arch_setup(); in bl2_el3_setup [bl2_main]\n");
	/* Perform late platform-specific setup */
	bl2_el3_plat_arch_setup();
	ATF_BP(" FINISH: bl2_el3_plat_arch_setup(); in bl2_el3_setup [bl2_main]\n");

}
#else /* RESET_TO_BL2 */

/*******************************************************************************
 * Setup function for BL2 when RESET_TO_BL2=0
 ******************************************************************************/
void bl2_setup(u_register_t arg0, u_register_t arg1, u_register_t arg2,
	       u_register_t arg3)
{
	
	printf("Here we are in BL2\n");
	//ATF_BP(" Here is bl2_setup [bl2_main]\n");

	// ATF_BP(" START: plat_setup_early_console(); in bl2_setup [bl2_main]\n");

	/* Enable early console if EARLY_CONSOLE flag is enabled */
	plat_setup_early_console();
	//ATF_BP(" FINISH: plat_setup_early_console(); in bl2_setup [bl2_main]\n");

	//ATF_BP(" START: bl2_early_platform_setup2(arg0, arg1, arg2, arg3); in bl2_setup [bl2_main]\n");
	/* Perform early platform-specific setup */
	bl2_early_platform_setup2(arg0, arg1, arg2, arg3);
	ATF_BP(" FINISH: bl2_early_platform_setup2(arg0, arg1, arg2, arg3); in bl2_setup [bl2_main]\n");
	////atf_testpoint_print_RegGroup3_in_BL2_EL1();

	//smc(BL1_SMC_RUN_IMAGE, 0, 0, 0, 0, 0, 0, 0);

	ATF_BP(" START: bl2_plat_arch_setup(); in bl2_setup [bl2_main]\n");
	/* Perform late platform-specific setup */
	bl2_plat_arch_setup();
	ATF_BP(" FINISH: bl2_plat_arch_setup(); in bl2_setup [bl2_main]\n");
	//atf_testpoint_print_RegGroup1();
	//atf_testpoint_print_RegGroup3();


}
#endif /* RESET_TO_BL2 */

/*******************************************************************************
 * The only thing to do in BL2 is to load further images and pass control to
 * next BL. The memory occupied by BL2 will be reclaimed by BL3x stages. BL2
 * runs entirely in S-EL1.
 ******************************************************************************/
void bl2_main(void)
{
	entry_point_info_t *next_bl_ep_info;
	//smc(BL1_SMC_RUN_IMAGE, 0, 0, 0, 0, 0, 0, 0);


#if ENABLE_RUNTIME_INSTRUMENTATION
	PMF_CAPTURE_TIMESTAMP(bl_svc, BL2_ENTRY, PMF_CACHE_MAINT);
#endif

	NOTICE("BL2: %s\n", build_version_string);
	NOTICE("BL2: %s\n", build_message);

	/* Perform remaining generic architectural setup in S-EL1 */
	ATF_BP(" START: bl2_arch_setup(); in bl2_main [bl2_main]\n");

	bl2_arch_setup();

	ATF_BP(" FINISH: bl2_arch_setup(); in bl2_main [bl2_main]\n");
	//atf_testpoint_print_RegGroup3_in_BL2_EL1();
	//atf_testpoint_print_RegGroup3();



#if PSA_FWU_SUPPORT
	fwu_init();
#endif /* PSA_FWU_SUPPORT */
	ATF_BP(" START: crypto_mod_init(); in bl2_main [bl2_main]\n");
	crypto_mod_init();
	ATF_BP(" FINISH: crypto_mod_init(); in bl2_main [bl2_main]\n");
	//atf_testpoint_print_RegGroup3_in_BL2_EL1();
	//atf_testpoint_print_RegGroup3();

	ATF_BP(" START: auth_mod_init(); in bl2_main [bl2_main]\n");
	/* Initialize authentication module */
	auth_mod_init();
	ATF_BP(" FINISH: auth_mod_init(); in bl2_main [bl2_main]\n");
	//atf_testpoint_print_RegGroup3_in_BL2_EL1();
	//atf_testpoint_print_RegGroup3();

	ATF_BP(" START: bl2_plat_mboot_init(); in bl2_main [bl2_main]\n");
	/* Initialize the Measured Boot backend */
	bl2_plat_mboot_init();
	ATF_BP(" FINISH: bl2_plat_mboot_init(); in bl2_main [bl2_main]\n");
	//atf_testpoint_print_RegGroup3_in_BL2_EL1();
	//atf_testpoint_print_RegGroup3();


	ATF_BP(" START: bl2_plat_preload_setup(); in bl2_main [bl2_main]\n");
	/* Initialize boot source */
	bl2_plat_preload_setup();
	ATF_BP(" FINISH: bl2_plat_preload_setup(); in bl2_main [bl2_main]\n");
	//atf_testpoint_print_RegGroup3_in_BL2_EL1();
	//atf_testpoint_print_RegGroup3();

	ATF_BP(" START: bl2_load_images(); in bl2_main [bl2_main]\n");
	/* Load the subsequent bootloader images. */
	next_bl_ep_info = bl2_load_images();
	//ATF_BP(" START: next_bl_ep_info is [bl2_main]\n");

	ATF_BP(" FINISH: bl2_load_images(); in bl2_main [bl2_main]\n");
	//atf_testpoint_print_RegGroup3_in_BL2_EL1();
	//atf_testpoint_print_RegGroup3();


	ATF_BP(" START: bl2_plat_mboot_finish(); in bl2_main [bl2_main]\n");
	/* Teardown the Measured Boot backend */
	bl2_plat_mboot_finish();
	ATF_BP(" FINISH: bl2_plat_mboot_finish(); in bl2_main [bl2_main]\n");
	//atf_testpoint_print_RegGroup3();

	ATF_BP(" START: crypto_mod_finish(); in bl2_main [bl2_main]\n");
	crypto_mod_finish();
	ATF_BP(" FINISH: crypto_mod_finish(); in bl2_main [bl2_main]\n");
	//atf_testpoint_print_RegGroup3_in_BL2_EL1();
	//atf_testpoint_print_RegGroup3();


#if !BL2_RUNS_AT_EL3
#ifndef __aarch64__
	/*
	 * For AArch32 state BL1 and BL2 share the MMU setup.
	 * Given that BL2 does not map BL1 regions, MMU needs
	 * to be disabled in order to go back to BL1.
	 */
	ATF_BP(" START: disable_mmu_icache_secure(); in bl2_main [bl2_main]\n");
	
	disable_mmu_icache_secure();
	ATF_BP(" FINISH: disable_mmu_icache_secure(); in bl2_main [bl2_main]\n");

#endif /* !__aarch64__ */

	/*
	 * Disable pointer authentication before running next boot image
	 */
	if (is_feat_pauth_supported()) {
		ATF_BP(" START: pauth_disable_el1(); in bl2_main [bl2_main]\n");

		pauth_disable_el1();

		ATF_BP(" FINISH: pauth_disable_el1(); in bl2_main [bl2_main]\n");

	}

#if ENABLE_RUNTIME_INSTRUMENTATION
	ATF_BP(" START: PMF_CAPTURE_TIMESTAMP(bl_svc, BL2_EXIT, PMF_CACHE_MAINT); in bl2_main [bl2_main]\n");

	PMF_CAPTURE_TIMESTAMP(bl_svc, BL2_EXIT, PMF_CACHE_MAINT);

	ATF_BP(" FINISH: PMF_CAPTURE_TIMESTAMP(bl_svc, BL2_EXIT, PMF_CACHE_MAINT); in bl2_main [bl2_main]\n");

#endif
	ATF_BP(" START: console_flush(); in bl2_main [bl2_main]\n");

	console_flush();
	ATF_BP(" FINISH: console_flush(); in bl2_main [bl2_main]\n");

	/*
	 * Run next BL image via an SMC to BL1. Information on how to pass
	 * control to the BL32 (if present) and BL33 software images will
	 * be passed to next BL image as an argument.
	 */



	 
	//atf_testpoint_print_RegGroup3();

	bl2_run_next_image(next_bl_ep_info);




	ATF_BP(" START: smc(BL1_SMC_RUN_IMAGE, (unsigned long)next_bl_ep_info, 0, 0, 0, 0, 0, 0); in bl2_main [bl2_main]\n");

	smc(BL1_SMC_RUN_IMAGE, (unsigned long)next_bl_ep_info, 0, 0, 0, 0, 0, 0);
	ATF_BP(" FINISH: smc(BL1_SMC_RUN_IMAGE, (unsigned long)next_bl_ep_info, 0, 0, 0, 0, 0, 0); in bl2_main [bl2_main]\n");

	//atf_testpoint_print_RegGroup3();

#else /* if BL2_RUNS_AT_EL3 */

	NOTICE("BL2: Booting " NEXT_IMAGE "\n");
	print_entry_point_info(next_bl_ep_info);
#if ENABLE_RUNTIME_INSTRUMENTATION
	PMF_CAPTURE_TIMESTAMP(bl_svc, BL2_EXIT, PMF_CACHE_MAINT);
#endif
	console_flush();

	/*
	 * Disable pointer authentication before running next boot image
	 */
	if (is_feat_pauth_supported()) {
		pauth_disable_el3();
	}

	bl2_run_next_image(next_bl_ep_info);
#endif /* BL2_RUNS_AT_EL3 */
}
