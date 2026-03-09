/*
 * Copyright (c) 2018, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <asm_macros.S>
#include <assert_macros.S>
#include <lib/xlat_tables/xlat_tables_v2.h>

	.global	enable_mmu_direct_el1
	.global	enable_mmu_direct_el2
	.global	enable_mmu_direct_el3

	/* Macros to read and write to system register for a given EL. */
	.macro _msr reg_name, el, gp_reg
	msr	\reg_name\()_el\()\el, \gp_reg
	.endm

	.macro _mrs gp_reg, reg_name, el
	mrs	\gp_reg, \reg_name\()_el\()\el
	.endm

	.macro tlbi_invalidate_all el
	.if \el == 1
		TLB_INVALIDATE(vmalle1)
	.elseif \el == 2
		TLB_INVALIDATE(alle2)
	.elseif \el == 3
		TLB_INVALIDATE(alle3)
	.else
		.error "EL must be 1, 2 or 3"
	.endif
	.endm

	/* void enable_mmu_direct_el<x>(unsigned int flags) */
	.macro define_mmu_enable_func el
	func enable_mmu_direct_\()el\el
/*
	msr	daifset,#0xf
	dsb	sy
	isb
	mov	x0, #DCCISW
	bl	dcsw_op_all
	dsb	sy
	isb
	*/
#if ENABLE_ASSERTIONS
		_mrs	x1, sctlr, \el
		tst	x1, #SCTLR_M_BIT
		ASM_ASSERT(eq)
#endif
		/* Invalidate all TLB entries */
		tlbi_invalidate_all \el

		mrs x7, S3_6_C15_C8_0
		orr x7, x7, #0x1
		msr S3_6_C15_C8_0, x7
		dsb ish
		isb
		
		//mov	x17, x30
/*		
		mov	x0, x30
		bl	atf_testpoint_printReg_hex
		mrs	x0, ID_AA64ISAR1_EL1
		bl	atf_testpoint_printReg_hex
*/

		mov	x7, x0
		adrp	x0, mmu_cfg_params
		add	x0, x0, :lo12:mmu_cfg_params

		/* MAIR */
		ldr	x1, [x0, #(MMU_CFG_MAIR << 3)]
		_msr	mair, \el, x1

		/* TCR */
		ldr	x2, [x0, #(MMU_CFG_TCR << 3)]
		_msr	tcr, \el, x2

		/* TTBR */
		ldr	x3, [x0, #(MMU_CFG_TTBR0 << 3)]
		_msr	ttbr0, \el, x3

		/*bl	atf_testpoint_x03*/


		/*
		 * Ensure all translation table writes have drained into memory, the TLB
		 * invalidation is complete, and translation register writes are
		 * committed before enabling the MMU
		 */
		dsb	ish
		isb
/*
		//[Change]Clean I-cache 
		ic	iallu
		dsb	sy
		isb
*/
		/*bl	atf_testpoint_test*/


		/* Set and clear required fields of SCTLR */
		
		_mrs	x4, sctlr, \el
		//mov_imm	x5, SCTLR_WXN_BIT | SCTLR_C_BIT 
		//mov_imm	x5, SCTLR_WXN_BIT | SCTLR_C_BIT | SCTLR_M_BIT
		//mov_imm	x5, SCTLR_WXN_BIT | SCTLR_M_BIT 
		//mov_imm	x5, SCTLR_WXN_BIT | SCTLR_M_BIT
		mov_imm	x5, SCTLR_C_BIT | SCTLR_M_BIT
		orr	x4, x4, x5
		

		/* Additionally, amend SCTLR fields based on flags */
		
		bic	x5, x4, #SCTLR_C_BIT
/*
		bl	atf_testpoint_test
		mov	x10, x0
		mov	x14, x4
		mov	x15, x5	
		mov	x0, x4
		bl	atf_testpoint_printReg_hex
		mov	x0, x5
		bl	atf_testpoint_printReg_hex
		mov	x0, x10
		mov	x4, x14
		mov	x5, x15
*/

		tst	x7, #DISABLE_DCACHE

		csel	x4, x5, x4, ne


/*
		mov	x10, x0
		mov	x14, x4
		mov	x15, x5	
		mov	x0, x4
		bl	atf_testpoint_printReg_hex
		mov	x0, x5
		bl	atf_testpoint_printReg_hex
		mov	x0, x10
		mov	x4, x14
		mov	x5, x15
*/
/*
		bl	atf_testpoint_test
		mov	x0, x4
		bl	atf_testpoint_printReg_hex

		mrs	x0, ttbr0_el3
		bl	atf_testpoint_printReg_hex
		ldr	x0, =msg_tbbr0
		bl	atf_testpoint_printReg_hex

		mrs	x0, mair_el3
		bl	atf_testpoint_printReg_hex
		ldr	x0, =msg_mair

		bl	atf_testpoint_printReg_hex

		mrs	x0, tcr_el3
		bl	atf_testpoint_printReg_hex
		ldr	x0, =msg_tcr
		bl	atf_testpoint_printReg_hex

		mrs	x0, sctlr_el3
		bl	atf_testpoint_printReg_hex
		ldr	x0, =msg_sctlr
		bl	atf_testpoint_printReg_hex

		adr	x0, .
		bl	atf_testpoint_printReg_hex
		ldr	x0, =msg_pc
		bl	atf_testpoint_printReg_hex

		mov	x0, sp
		bl	atf_testpoint_printReg_hex
		ldr	x0, =msg_sp
		bl	atf_testpoint_printReg_hex

		mov	x0, x4
		bl	atf_testpoint_printReg_hex
*/
/*
		mov	x0, sp
		bl	atf_testpoint_printReg_hex
		mov	x0, sp
		bl	atf_testpoint_printAddVal
		mov	x0, sp
		bl	atf_testpoint_printAddVal_double
		mov	x0, sp
		bl	atf_testpoint_printAddVal_tri
		mov	x0, x30
		bl	atf_testpoint_printReg_hex
*/
		_msr	sctlr, \el, x4
/*		
		bl	atf_testpoint_test
		bl	atf_testpoint_test2
*/

		//mov	x0, x17
/*		
		bl	atf_testpoint_printReg_hex
*/
/*
		tlbi	vmalle1is
		dsb	sy
*/
		dsb	sy
		isb sy
/*
		_mrs	x4, sctlr, \el
		mov_imm	x5, SCTLR_I_BIT | SCTLR_C_BIT | SCTLR_M_BIT
		orr	x4, x4, x5
		_msr	sctlr, \el, x4
		isb
*/
/*		
		bl	atf_testpoint_test
		bl	atf_testpoint_test2
*/

		/*bl	atf_testpoint_test*/
		

		/*mov_imm	x30, 0x3620*/

		//mov	x30, x17



		ret
		//br	x30
	endfunc enable_mmu_direct_\()el\el
	.endm

	/*
	 * Define MMU-enabling functions for EL1, EL2 and EL3:
	 *
	 *  enable_mmu_direct_el1
	 *  enable_mmu_direct_el2
	 *  enable_mmu_direct_el3
	 */
	define_mmu_enable_func 1
	define_mmu_enable_func 2
	define_mmu_enable_func 3
/*
.section .rodata
msg_tbbr0: .asciz "TTBR0_EL3: "
msg_mair:  .asciz "MAIR_EL3: "
msg_tcr:   .asciz "TCR_EL3: "
msg_sctlr: .asciz "SCTLR_EL3: "
msg_pc:    .asciz "PC: "
msg_sp:    .asciz "SP: "
*/