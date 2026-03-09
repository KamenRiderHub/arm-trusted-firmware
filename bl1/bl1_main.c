/*
 * Copyright (c) 2013-2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#include <assert.h>

#include <platform_def.h>

#include <arch.h>
#include <arch_features.h>
#include <arch_helpers.h>
#include <bl1/bl1.h>
#include <common/bl_common.h>
#include <common/build_message.h>
#include <common/debug.h>
#include <drivers/auth/auth_mod.h>
#include <drivers/auth/crypto_mod.h>
#include <drivers/console.h>
#include <lib/bootmarker_capture.h>
#include <lib/cpus/errata.h>
#include <lib/pmf/pmf.h>
#include <lib/utils.h>
#include <plat/common/platform.h>
#include <smccc_helpers.h>
#include <tools_share/uuid.h>

#include "bl1_private.h"
#include "plat/arm/board/cx/cx_uart.h"
#include "plat/arm/board/cx/cx_console.h"
#include <lib/mmio.h>



static void bl1_load_bl2(void);

#if ENABLE_PAUTH
uint64_t bl1_apiakey[2];
#endif

#if ENABLE_RUNTIME_INSTRUMENTATION
	PMF_REGISTER_SERVICE(bl_svc, PMF_RT_INSTR_SVC_ID,
		BL_TOTAL_IDS, PMF_DUMP_ENABLE)
#endif

/*******************************************************************************
 * Setup function for BL1.
 ******************************************************************************/
void atf_reg_test_smmu()
{
	uint32_t tmp_base = 0x0d000000;
	uint32_t tmp_read = 0x0d000000;
	tmp_read+=0;

	// 1.
	tmp_read = mmio_read_32(tmp_base + 0x90);
    mmio_write_32(tmp_base + 0x90, 0x4);
	tmp_read = mmio_read_32(tmp_base + 0x90);
	if(tmp_read!=0x4)
	{
		printf("Error!1");
	}

	// 2.
	tmp_read = mmio_read_32(tmp_base + 0x94);
    mmio_write_32(tmp_base + 0x94, 0x0);
	tmp_read = mmio_read_32(tmp_base + 0x94);
	if(tmp_read!=0x0)
	{
		printf("Error!2");
	}
	// 3.
	tmp_read = mmio_read_32(tmp_base + 0x9c);
    mmio_write_32(tmp_base + 0x9c, 0x0);
	tmp_read = mmio_read_32(tmp_base + 0x9c);
	if(tmp_read!=0x0)
	{
		printf("Error!3");
	}
	// 4.
	tmp_read = mmio_read_32(tmp_base + 0x98);
    mmio_write_32(tmp_base + 0x98, 0x0);
	tmp_read = mmio_read_32(tmp_base + 0x98);
	if(tmp_read!=0x0)
	{
		printf("Error!4");
	}
	// 5.
	tmp_read = mmio_read_32(tmp_base + 0x8020);
    mmio_write_32(tmp_base + 0x8020, 0x8);
	tmp_read = mmio_read_32(tmp_base + 0x8020);
	if(tmp_read!=0x8)
	{
		printf("Error!5");
	}
	// 6.
	tmp_read = mmio_read_32(tmp_base + 0x8024);

	// 7.
	tmp_read = mmio_read_32(tmp_base + 0xa0);
    mmio_write_32(tmp_base + 0xa0, 0x2004);
	tmp_read = mmio_read_32(tmp_base + 0xa0);
	if(tmp_read!=0x2004)
	{
		printf("Error!7");
	}
	// 8.
	tmp_read = mmio_read_32(tmp_base + 0xa4);
    mmio_write_32(tmp_base + 0xa4, 0x0);
	tmp_read = mmio_read_32(tmp_base + 0xa4);
	if(tmp_read!=0x0)
	{
		printf("Error!8");
	}
	// 9.
	tmp_read = mmio_read_32(tmp_base + 0x100ac);
    mmio_write_32(tmp_base + 0x100ac, 0x0);
	tmp_read = mmio_read_32(tmp_base + 0x100ac);
	if(tmp_read!=0x0)
	{
		printf("Error!9");
	}
	// 10.
	tmp_read = mmio_read_32(tmp_base + 0x100a8);
    mmio_write_32(tmp_base + 0x100a8, 0x0);
	tmp_read = mmio_read_32(tmp_base + 0x100a8);
	if(tmp_read!=0x0)
	{
		printf("Error!10");
	}
	// 11.
	tmp_read = mmio_read_32(tmp_base + 0x8020);
    mmio_write_32(tmp_base + 0x8020, 0xc);
	tmp_read = mmio_read_32(tmp_base + 0x8020);
	if(tmp_read!=0xc)
	{
		printf("Error!11");
	}
	// 12.
	tmp_read = mmio_read_32(tmp_base + 0x8024);

	// 13.
	tmp_read = mmio_read_32(tmp_base + 0x80);
    mmio_write_32(tmp_base + 0x80, 0x4000);
	tmp_read = mmio_read_32(tmp_base + 0x80);
	if(tmp_read!=0x4000)
	{
		printf("Error!13");
	}
	// 14.
	tmp_read = mmio_read_32(tmp_base + 0x84);
    mmio_write_32(tmp_base + 0x84, 0x0);
	tmp_read = mmio_read_32(tmp_base + 0x84);
	if(tmp_read!=0x0)
	{
		printf("Error!14");
	}
	// 15.
	tmp_read = mmio_read_32(tmp_base + 0x88);
    mmio_write_32(tmp_base + 0x88, 0x4);
	tmp_read = mmio_read_32(tmp_base + 0x88);
	if(tmp_read!=0x4)
	{
		printf("Error!15");
	}
	// 16.
	tmp_read = mmio_read_32(tmp_base + 0x20);
    mmio_write_32(tmp_base + 0x20, 0x1);
	tmp_read = mmio_read_32(tmp_base + 0x20);
	if(tmp_read!=0x1)
	{
		printf("Error!16");
	}
	// 17.
	tmp_read = mmio_read_32(tmp_base + 0x24);

	tmp_read+=0;
}

void atf_reg_test_inSoft()
{
    uint32_t tmp_use = 0;
	//printf(" --------------------- [Register Test] --------------------- \n");
	uint32_t tmp_uart1 = mmio_read_32(0x0c001000 + CXUART_LCR_OFFSET);
	//printf(" Read 0x%x is 0x%x\n", 0x0c001000 + CXUART_LCR_OFFSET, tmp_uart1);
	//printf(" Write {0x%x}on 0x%x [UART1]\n", (tmp_uart1|LCR_DLAB_CTRL), 0x0c001000 + CXUART_LCR_OFFSET);
    mmio_write_32(0x0c001000 + CXUART_LCR_OFFSET, (tmp_uart1|LCR_DLAB_CTRL));
    tmp_use = mmio_read_32(0x0c001000 + CXUART_LCR_OFFSET);
	if(tmp_use!=(tmp_uart1|LCR_DLAB_CTRL))
	{
		printf("Error!uart1");
	}
	//printf(" Read 0x%x is 0x%x\n", 0x0c001000 + CXUART_LCR_OFFSET, mmio_read_32(0x0c001000 + CXUART_LCR_OFFSET));
    mmio_write_32(0x0c001000 + CXUART_LCR_OFFSET, tmp_uart1);//恢复
    tmp_use = mmio_read_32(0x0c001000 + CXUART_LCR_OFFSET);
    
	//printf(" [Recover] Read 0x%x is 0x%x\n", 0x0c001000 + CXUART_LCR_OFFSET, mmio_read_32(0x0c001000 + CXUART_LCR_OFFSET));
	
	// uint32_t tmp_uart1_2 = mmio_read_32(0x0c001000);
	
	// //printf(" [Reverse]Write {0x%x} on 0x%x [UART1]\n", (~tmp_uart1_2), 0x0c001000);
    // mmio_write_32(0x0c001000, (~tmp_uart1_2));
	// //printf(" [Reverse] Read 0x%x is 0x%x\n", 0x0c001000, mmio_read_32(0x0c001000));
    // mmio_write_32(0x0c001000, tmp_uart1_2);//恢复
    
	
	//printf(" =========================================================== \n");
	
	////printf(" Write {0x12345678} on 0x0c001000 [UART1]\n");
	//mmio_write_32(0x0c001000, 0x12345678);
	////printf(" Read 0x0c001000 is 0x%x\n",mmio_read_32(0x0c001000));
	tmp_use = mmio_read_8(0x0c004000);
	//printf(" Read 0x0c004000 is 0x%x\n",mmio_read_8(0x0c004000));	
	//printf(" Write {0x12} on 0x0c004000 [GPIO0]\n");
	mmio_write_8(0x0c004000, 0x12);
	tmp_use = mmio_read_8(0x0c004000);
	if(tmp_use!=0x12)
	{
		printf("Error!GPIO0");
	}
	//printf(" Read 0x0c004000 is 0x%x\n",mmio_read_8(0x0c004000));
	//printf(" =========================================================== \n");

	tmp_use = mmio_read_8(0x0c005000);

	//printf(" Read 0x0c005000 is 0x%x\n",mmio_read_8(0x0c005000));
	//printf(" Write {0x12} on 0x0c005000 [GPIO1]\n");
	mmio_write_8(0x0c005000, 0x12);
	tmp_use = mmio_read_8(0x0c005000);
	if(tmp_use!=0x12)
	{
		printf("Error!GPIO1");
	}
    
	//printf(" Read 0x0c005000 is 0x%x\n",mmio_read_8(0x0c005000));
	//printf(" =========================================================== \n");

/*
	//printf(" Write {0x12345678} on 0x0c004000 [GPIO0]\n");
	mmio_write_32(0x0c004000, 0x12345678);
	//printf(" Read 0x0c004000 is 0x%x\n",mmio_read_32(0x0c004000));
	
	//printf(" Write {0x12345678} on 0x0c005000 [GPIO1]\n");
	mmio_write_32(0x0c005000, 0x12345678);
	//printf(" Read 0x0c005000 is 0x%x\n",mmio_read_32(0x0c005000));
*/
	tmp_use = mmio_read_8(0x0c006000);

    //printf(" Read 0x0c006000 is 0x%x\n",mmio_read_8(0x0c006000));
    //printf(" Write {0x34} on 0x0c006000 [I2C0]\n");
    mmio_write_8(0x0c006000, 0x34);
	tmp_use = mmio_read_8(0x0c006000);
	if(tmp_use!=0x34)
	{
		printf("Error!I2C0");
	}
    
    //printf(" Read 0x0c006000 is 0x%x\n",mmio_read_8(0x0c006000));
	//printf(" =========================================================== \n");

	tmp_use = mmio_read_8(0x0c007000);
    
    //printf(" Read 0x0c007000 is 0x%x\n",mmio_read_8(0x0c007000));
    //printf(" Write {0x34} on 0x0c007000 [I2C1]\n");
    mmio_write_8(0x0c007000, 0x34);
	tmp_use = mmio_read_8(0x0c007000);
	if(tmp_use!=0x34)
	{
		printf("Error!I2C1");
	}
    
    //printf(" Read 0x0c007000 is 0x%x\n",mmio_read_8(0x0c007000));
	//printf(" =========================================================== \n");

	tmp_use = mmio_read_8(0x0c008000);
    
    //printf(" Read 0x0c008000 is 0x%x\n",mmio_read_8(0x0c008000));
    //printf(" Write {0x34} on 0x0c008000 [I2C2]\n");
    mmio_write_8(0x0c008000, 0x34);
	tmp_use = mmio_read_8(0x0c008000);
   	if(tmp_use!=0x34)
	{
		printf("Error!I2C2");
	} 
    //printf(" Read 0x0c008000 is 0x%x\n",mmio_read_8(0x0c008000));
	//printf(" =========================================================== \n");

/*
    //printf(" Write {0x12345678} on 0x0c006000 [I2C0]\n");
    mmio_write_32(0x0c006000, 0x12345678);
    //printf(" Read 0x0c006000 is 0x%x\n",mmio_read_32(0x0c006000));

    //printf(" Write {0x12345678} on 0x0c007000 [I2C1]\n");
    mmio_write_32(0x0c007000, 0x12345678);
    //printf(" Read 0x0c007000 is 0x%x\n",mmio_read_32(0x0c007000));

    //printf(" Write {0x12345678} on 0x0c008000 [I2C2]\n");
    mmio_write_32(0x0c008000, 0x12345678);
    //printf(" Read 0x0c008000 is 0x%x\n",mmio_read_32(0x0c008000));
*/
    //printf(" [NO I3C]\n");
    //mmio_write_32(0x0c009000, 0x12345678);
    ////printf(" Read 0x0c009000 is 0x%x\n",mmio_read_32(0x0c009000));
	//printf(" =========================================================== \n");

	tmp_use = mmio_read_8(0x0c00a000);
    
    //printf(" Read 0x0c00a000 is 0x%x\n",mmio_read_8(0x0c00a000));
    uint8_t tmp_wdt=mmio_read_8(0x0c00a000);
    //printf(" Write {0x%x} on 0x0c00a000 [WDT]\n",(tmp_wdt|(1<<5)));
    mmio_write_8(0x0c00a000, (tmp_wdt|(1<<5)));
	tmp_use = mmio_read_8(0x0c00a000);
    
    //printf(" Read 0x0c00a000 is 0x%x\n",mmio_read_8(0x0c00a000));
	//printf(" =========================================================== \n");

	tmp_use = mmio_read_32(0x0c00c000);
    
    uint32_t tmp_spi = mmio_read_32(0x0c00c000);
    //printf(" Read 0x0c00c000 is 0x%x\n",tmp_spi);
    //printf(" Write {0x%x} on 0x0c00c000 [SPI]\n",(~tmp_spi));
    mmio_write_32(0x0c00c000, (~tmp_spi));
	tmp_use = mmio_read_32(0x0c00c000);
    
    //printf(" Read 0x0c00c000 is 0x%x\n",mmio_read_32(0x0c00c000));
    mmio_write_32(0x0c00c000, tmp_spi); //恢复
	tmp_use = mmio_read_32(0x0c00c000);
    
    //printf(" [Recover] Read 0x0c00c000 is 0x%x\n",mmio_read_32(0x0c00c000));   
	//printf(" =========================================================== \n");

	tmp_use = mmio_read_32(0x0c00d000);
    
    //printf(" Read 0x0c00d000 is 0x%x\n",mmio_read_32(0x0c00d000));
    //printf(" Write {0x12345678} on 0x0c00d000 [timer]\n");
    mmio_write_32(0x0c00d000, 0x12345678);
	tmp_use = mmio_read_32(0x0c00d000);
    
    //printf(" Read 0x0c00d000 is 0x%x\n",mmio_read_32(0x0c00d000));
	//printf(" =========================================================== \n");

	tmp_use = mmio_read_32(0x0c014000);
    
    uint32_t tmp_qspi = mmio_read_32(0x0c014000);
    //printf(" Read 0x0c014000 is 0x%x\n",tmp_qspi);
    //printf(" Write {0x%x} on 0x0c014000 [QSPI]\n",(~tmp_qspi));
    mmio_write_32(0x0c014000, (~tmp_qspi));
	tmp_use = mmio_read_32(0x0c014000);
    
    //printf(" Read 0x0c014000 is 0x%x\n",mmio_read_32(0x0c014000));
    mmio_write_32(0x0c014000, tmp_qspi); //恢复
	tmp_use = mmio_read_32(0x0c014000);
    
    //printf(" [Recover] Read 0x0c014000 is 0x%x\n",mmio_read_32(0x0c014000));       
	//printf(" =========================================================== \n");
 
	// tmp_use = mmio_read_32(0x0d000044);
 
    // uint32_t tmp_smmu = mmio_read_32(0x0d000044);
    // //printf(" Read 0x0d000044 is 0x%x\n",tmp_smmu);
    // //printf(" Write {0x%x} on 0x0d000044 [SMMU]\n", ((tmp_smmu)|(1U << 31)|(1U << 20)));
    // mmio_write_32(0x0d000044, ((tmp_smmu)|(1U << 31)|(1U << 20)));
	// tmp_use = mmio_read_32(0x0d000044);
    
    // //printf(" Read 0x0d000044 is 0x%x\n",mmio_read_32(0x0d000044));   
    // mmio_write_32(0x0d000044, tmp_smmu); //恢复
	// tmp_use = mmio_read_32(0x0d000044);
    
    //printf(" [Recover] Read 0x0d000044 is 0x%x\n",mmio_read_32(0x0d000044));   
    
	//printf(" =========================================================== \n");
    //atf_reg_test_smmu();

	tmp_use = mmio_read_32(0x0e000000);
 
    uint32_t tmp_gic = mmio_read_32(0x0e000000);
    //printf(" Read 0x0e000000 is 0x%x\n",tmp_gic);
    //printf(" Write {0x%x} on 0x0e000000 [GIC]\n", ((tmp_gic)|(3U << 28)));
    // mmio_write_32(0x0e000000, ((tmp_gic)|(3U << 28)));
    // mmio_write_32(0x0e000000, ~tmp_gic);

	tmp_use = mmio_read_32(0x0e000000);
	// printf("~tmp_gic is 0x%x\n",~tmp_gic);
	// printf("tmp_use is 0x%x\n",tmp_use);

	/*
	if(tmp_use!=((tmp_gic)|(3U << 28)))
	{
		printf("Error!GIC");
	}
    */
    //printf(" Read 0x0e000000 is 0x%x\n",mmio_read_32(0x0e000000));   
    mmio_write_32(0x0e000000, tmp_gic); //恢复
	tmp_use = mmio_read_32(0x0e000000);
    
    //printf(" [Recover] Read 0x0e000000 is 0x%x\n",mmio_read_32(0x0e000000));   
    
	//printf(" =========================================================== \n");
 
	tmp_use = mmio_read_32(0x0f000000);
    
    //printf(" Read 0x0f000000 is 0x%x\n",mmio_read_32(0x0f000000));
    //printf(" Write {0x12345678} on 0x0f000000 [sram(64KB)(SBSX)]\n");
    
    //mmio_write_32(0x0f000000, 0x12345678);
    
    mmio_write_32(0x0f000000, 0x5A5A5A5A);
	tmp_use = mmio_read_32(0x0f000000);
	if(tmp_use!=0x5A5A5A5A)
	{
		printf("Error!5A-1");
	}
    mmio_write_32(0x0f000000, 0xA5A5A5A5);
	tmp_use = mmio_read_32(0x0f000000);
	if(tmp_use!=0xA5A5A5A5)
	{
		printf("Error!A5-1");
	}

    mmio_write_32(0x0f000004, 0x5A5A5A5A);
	tmp_use = mmio_read_32(0x0f000004);
	if(tmp_use!=0x5A5A5A5A)
	{
		printf("Error!5A-2");
	}
    mmio_write_32(0x0f000004, 0xA5A5A5A5);
	tmp_use = mmio_read_32(0x0f000004);
	if(tmp_use!=0xA5A5A5A5)
	{
		printf("Error!A5-2");
	}
    
    mmio_write_32(0x0f000008, 0x5A5A5A5A);
	tmp_use = mmio_read_32(0x0f000008);
	if(tmp_use!=0x5A5A5A5A)
	{
		printf("Error!5A-3");
	}
    mmio_write_32(0x0f000008, 0xA5A5A5A5);
	tmp_use = mmio_read_32(0x0f000008);
	if(tmp_use!=0xA5A5A5A5)
	{
		printf("Error!A5-3");
	}
    
    tmp_use+=0;
    //printf(" Read 0x0f000000 is 0x%x\n",mmio_read_32(0x0f000000));
	//printf(" =========================================================== \n");
	
	
    //printf(" --------------------- [Register Test] --------------------- \n");
}

#define FIP_ADDR        0x04000000UL
#define BL1bin_ADDR        0x00000000UL
#define BL1bin2_ADDR        0x00000520UL
#define BLMEM_ADDR        0x84000000UL

#define FIP_READ_LEN    256    // 读取256字节，可以调整

// void dump_fip_bin_contents(void)
// {
//     volatile uint8_t *ptr = (volatile uint8_t *)FIP_ADDR;
//     size_t i;
//     printf("Dumping %u bytes from 0x%08lx:\n", FIP_READ_LEN, (unsigned long)FIP_ADDR);
//     for (i = 0; i < FIP_READ_LEN; ++i) {
//         printf("%02X ", ptr[i]);
//         if ((i + 1) % 16 == 0) printf("\n");
//     }
//     printf("\n");
// }
// void write_fip_bin_contents(void)
// {
//     volatile uint8_t *ptr = (volatile uint8_t *)FIP_ADDR;
//     size_t i;
//     printf("WRITING %u bytes from 0x%08lx:\n", FIP_READ_LEN, (unsigned long)FIP_ADDR);
//     for (i = 0; i < FIP_READ_LEN; ++i) {
// 		ptr[i]=(i%2==0)?0xAB:0xCC;
//     }
//     printf("------Write FIP_ADDR------\n");
// }

// void dump_fip_bin_contents1(void)
// {
//     volatile uint8_t *ptr = (volatile uint8_t *)BL1bin2_ADDR;
//     size_t i;
//     printf("Dumping %u bytes from 0x%08lx:\n", FIP_READ_LEN, (unsigned long)BL1bin2_ADDR);
//     for (i = 0; i < FIP_READ_LEN; ++i) {
//         printf("%02X ", ptr[i]);
//         if ((i + 1) % 16 == 0) printf("\n");
//     }
//     printf("\n");
// }

// void dump_fip_bin_contents2(void)
// {
//     volatile uint8_t *ptr = (volatile uint8_t *)BL1bin_ADDR;
//     size_t i;
//     printf("Dumping %u bytes from 0x%08lx:\n", FIP_READ_LEN, (unsigned long)BL1bin_ADDR);
//     for (i = 0; i < FIP_READ_LEN; ++i) {
//         printf("%02X ", ptr[i]);
//         if ((i + 1) % 16 == 0) printf("\n");
//     }
//     printf("\n");
// }

// void dump_fip_bin_contents3(void)
// {
//     volatile uint8_t *ptr = (volatile uint8_t *)BLMEM_ADDR;
//     size_t i;
//     printf("Dumping %u bytes from 0x%08lx:\n", FIP_READ_LEN, (unsigned long)BLMEM_ADDR);
//     for (i = 0; i < FIP_READ_LEN; ++i) {
//         printf("%02X ", ptr[i]);
//         if ((i + 1) % 16 == 0) printf("\n");
//     }
//     printf("\n");
// }

// void write_fip_bin_contents3(void)
// {
//     volatile uint8_t *ptr = (volatile uint8_t *)BLMEM_ADDR;
//     size_t i;
//     printf("WRITING %u bytes from 0x%08lx:\n", FIP_READ_LEN, (unsigned long)BLMEM_ADDR);
//     for (i = 0; i < FIP_READ_LEN; ++i) {
// 		ptr[i]=(i%2==0)?0xAB:0xCC;
//     }
//     printf("------Write FIP_ADDR------\n");
// }














/* Page table base address: you want them here */
#define L1_TABLE_BASE   0x81800000UL
#define L2_TABLE_BASE   0x81810000UL

/* Mapping range */
#define MAP_START       0x81000000UL
#define MAP_END         0x82000000UL   // not included
#define BLOCK_SIZE      (2UL * 1024UL * 1024UL)
void create_identity_ptable(void)
{
    uint64_t *l1 = (uint64_t *)L1_TABLE_BASE;
    uint64_t *l2 = (uint64_t *)L2_TABLE_BASE;

    INFO("Creating page table L1 @ 0x%lx\n", (unsigned long)L1_TABLE_BASE);
    INFO("Creating page table L2 @ 0x%lx\n", (unsigned long)L2_TABLE_BASE);

    /* ----------------------------------------------------------
     * STEP 1: Clear 64KB for L1 and L2 page tables
     * ---------------------------------------------------------- */
    //memset((void *)L1_TABLE_BASE, 0, 64 * 1024);
    //memset((void *)L2_TABLE_BASE, 0, 64 * 1024);

    /* ----------------------------------------------------------
     * STEP 2: L1 maps index for VA 0x8100_0000
     * ---------------------------------------------------------- */
    uint64_t l1_index = (MAP_START >> 30) & 0x1FF; /* (VA >> 30) */

    /* Put L2 table address into L1 entry:
     *  - bits[1:0] = 0b11 → next-level table
     */
    l1[l1_index] = (L2_TABLE_BASE & ~0xFFFULL) | 0b11;

    INFO("L1[%lu] = 0x%lx\n", l1_index, (unsigned long)l1[l1_index]);

    /* ----------------------------------------------------------
     * STEP 3: Fill L2 with 2MB block entries
     * ---------------------------------------------------------- */
    uint64_t va = MAP_START;
    uint64_t pa = MAP_START;
    uint64_t block_index = (va >> 21) & 0x1FF; // start L2 index = 0x81000000 / 2MB

    unsigned count = 0;

    while (pa < MAP_END)
    {
        /*
         * L2 block descriptor:
         *  bits[1:0] = 0b01  → block (2MB)
         *  bit[10] = AF=1 (Access Flag)
         */
		 //Stage 1
        uint64_t desc = (pa & ~((uint64_t)BLOCK_SIZE - 1)) |
                        (1ULL << 10) |     // AF = 1
                        0b01;              // block

		//Stage 2 
		// uint64_t desc = (pa & ~((uint64_t)BLOCK_SIZE - 1)) |
		// 		(1ULL << 10) |     // AF = 1
		// 		(1ULL << 7) |		// AP = 0b11
		// 		(1ULL << 6) |		// AP = 0b11
		// 		0b01;              // block

        l2[block_index] = desc;

        va += BLOCK_SIZE;
        pa += BLOCK_SIZE;
        block_index++;
        count++;
    }

    INFO("Created %u L2 block entries (2MB each)\n", count);

    INFO("Page table preparation complete.\n");
}



#define MAP_PA_START       0xD1000000UL
#define MAP_PA_END         0xD2000000UL   // not included

void create_identity_ptable_2(void)
{
    uint64_t *l1 = (uint64_t *)L1_TABLE_BASE;
    uint64_t *l2 = (uint64_t *)L2_TABLE_BASE;

    INFO("Creating page table L1 @ 0x%lx\n", (unsigned long)L1_TABLE_BASE);
    INFO("Creating page table L2 @ 0x%lx\n", (unsigned long)L2_TABLE_BASE);

    /* ----------------------------------------------------------
     * STEP 1: Clear 64KB for L1 and L2 page tables
     * ---------------------------------------------------------- */
    //memset((void *)L1_TABLE_BASE, 0, 64 * 1024);
    //memset((void *)L2_TABLE_BASE, 0, 64 * 1024);

    /* ----------------------------------------------------------
     * STEP 2: L1 maps index for VA 0x8100_0000
     * ---------------------------------------------------------- */
    uint64_t l1_index = (MAP_START >> 30) & 0x1FF; /* (VA >> 30) */

    /* Put L2 table address into L1 entry:
     *  - bits[1:0] = 0b11 → next-level table
     */
    l1[l1_index] = (L2_TABLE_BASE & ~0xFFFULL) | 0b11;

    INFO("L1[%lu] = 0x%lx\n", l1_index, (unsigned long)l1[l1_index]);

    /* ----------------------------------------------------------
     * STEP 3: Fill L2 with 2MB block entries
     * ---------------------------------------------------------- */
    uint64_t va = MAP_START;
    uint64_t pa = MAP_PA_START;
    uint64_t block_index = (va >> 21) & 0x1FF; // start L2 index = 0x81000000 / 2MB

    unsigned count = 0;

    while (pa < MAP_PA_END)
    {
        /*
         * L2 block descriptor:
         *  bits[1:0] = 0b01  → block (2MB)
         *  bit[10] = AF=1 (Access Flag)
         */
		 //Stage1
        uint64_t desc = (pa & ~((uint64_t)BLOCK_SIZE - 1)) |
                        (1ULL << 10) |     // AF = 1
                        0b01;              // block

		//Stage 2 
		// uint64_t desc = (pa & ~((uint64_t)BLOCK_SIZE - 1)) |
		// 		(1ULL << 10) |     // AF = 1
		// 		(1ULL << 7) |		// AP = 0b11
		// 		(1ULL << 6) |		// AP = 0b11
		// 		0b01;              // block

        l2[block_index] = desc;

        va += BLOCK_SIZE;
        pa += BLOCK_SIZE;
        block_index++;
        count++;
    }

    INFO("Created %u L2 block entries (2MB each)\n", count);

    INFO("Page table preparation complete.\n");
}


static int smmu_poll(uintptr_t smmu_reg, uint32_t mask, uint32_t value)
{
	uint32_t reg_val;
	while (true) 
	{
		reg_val = mmio_read_32(smmu_reg);
		if ((reg_val & mask) == value)
		{
			return 0;
		}
	}
}

void atf_reg_test_smmu02(void)
{
	// create_identity_ptable();
	create_identity_ptable_2();
    uint32_t base = 0x0d000000;

    //printf("SMMU register test start...\n");

    /* ====================== CMDQ_Config ====================== */

    /* 1. SMMU_CMDQ_BASE_LO @ 0x90 = 0x81000008 */
    mmio_write_32(base + 0x90, 0x81000008);
    if (mmio_read_32(base + 0x90) != 0x81000008)
        printf("Error! CMDQ_BASE_LO\n");

    /* 2. SMMU_CMDQ_BASE_HI @ 0x94 = 0x0 */
    mmio_write_32(base + 0x94, 0x00000000);
    if (mmio_read_32(base + 0x94) != 0x00000000)
        printf("Error! CMDQ_BASE_HI\n");

    /* 3. SMMU_CMDQ_CONS @ 0x9C = 0 */
    mmio_write_32(base + 0x9C, 0x00000000);
    if (mmio_read_32(base + 0x9C) != 0x00000000)
        printf("Error! CMDQ_CONS\n");

    /* 4. SMMU_CMDQ_PROD @ 0x98 = 0 */
    mmio_write_32(base + 0x98, 0x00000000);
    if (mmio_read_32(base + 0x98) != 0x00000000)
        printf("Error! CMDQ_PROD\n");


    // /* 1-B. SMMU_S_CMDQ_BASE_LO @ 0x8090 = 0x81000008 */
    // mmio_write_32(base + 0x8090, 0x81000008);
    // if (mmio_read_32(base + 0x8090) != 0x81000008)
    //     printf("Error! CMDQ_BASE_LO\n");

    // /* 2-B. SMMU_S_CMDQ_BASE_HI @ 0x8094 = 0x0 */
    // mmio_write_32(base + 0x8094, 0x00000000);
    // if (mmio_read_32(base + 0x8094) != 0x00000000)
    //     printf("Error! CMDQ_BASE_HI\n");

    // /* 3-B. SMMU_S_CMDQ_CONS @ 0x809C = 0 */
    // mmio_write_32(base + 0x809C, 0x00000000);
    // if (mmio_read_32(base + 0x809C) != 0x00000000)
    //     printf("Error! CMDQ_CONS\n");

    // /* 4-B. SMMU_S_CMDQ_PROD @ 0x8098 = 0 */
    // mmio_write_32(base + 0x8098, 0x00000000);
    // if (mmio_read_32(base + 0x8098) != 0x00000000)
    //     printf("Error! CMDQ_PROD\n");

    /* ====================== EventQ_Config ====================== */

    /* 5. SMMU_EVENTQ_BASE_LO @ 0xA0 = 0x81002007 */
    mmio_write_32(base + 0xA0, 0x81002007);
    if (mmio_read_32(base + 0xA0) != 0x81002007)
        printf("Error! EVENTQ_BASE_LO\n");

    /* 6. SMMU_EVENTQ_BASE_HI @ 0xA4 = 0 */
    mmio_write_32(base + 0xA4, 0x00000000);
    if (mmio_read_32(base + 0xA4) != 0x00000000)
        printf("Error! EVENTQ_BASE_HI\n");

    /* 7. SMMU_EVENTQ_CONS @ 0x100AC = 0 */
    mmio_write_32(base + 0x100AC, 0x00000000);
    if (mmio_read_32(base + 0x100AC) != 0x00000000)
        printf("Error! EVENTQ_CONS\n");

    /* 8. SMMU_EVENTQ_PROD @ 0x100A8 = 0 */
    mmio_write_32(base + 0x100A8, 0x00000000);
    if (mmio_read_32(base + 0x100A8) != 0x00000000)
        printf("Error! EVENTQ_PROD\n");


    // /* 5-B. SMMU_S_EVENTQ_BASE_LO @ 0x80A0 = 0x81002007 */
    // mmio_write_32(base + 0x80A0, 0x81002007);
    // if (mmio_read_32(base + 0x80A0) != 0x81002007)
    //     printf("Error! EVENTQ_BASE_LO\n");

    // /* 6-B. SMMU_S_EVENTQ_BASE_HI @ 0x80A4 = 0 */
    // mmio_write_32(base + 0x80A4, 0x00000000);
    // if (mmio_read_32(base + 0x80A4) != 0x00000000)
    //     printf("Error! EVENTQ_BASE_HI\n");

    // /* 7-B. SMMU_S_EVENTQ_CONS @ 0x80AC = 0 */
    // mmio_write_32(base + 0x80AC, 0x00000000);
    // if (mmio_read_32(base + 0x80AC) != 0x00000000)
    //     printf("Error! EVENTQ_CONS\n");

    // /* 8-B. SMMU_S_EVENTQ_PROD @ 0x80A8 = 0 */
    // mmio_write_32(base + 0x80A8, 0x00000000);
    // if (mmio_read_32(base + 0x80A8) != 0x00000000)
    //     printf("Error! EVENTQ_PROD\n");

    /* ====================== StrTab_Config ====================== */

    /* 9. SMMU_STRTAB_BASE_CFG @ 0x88 = 0x00000006 */
    mmio_write_32(base + 0x88, 0x00000006);
    if (mmio_read_32(base + 0x88) != 0x00000006)
        printf("Error! STRTAB_BASE_CFG\n");

    /* 10. SMMU_STRTAB_BASE_LO @ 0x80 = 0x81004000 */
    mmio_write_32(base + 0x80, 0x81004000);
    if (mmio_read_32(base + 0x80) != 0x81004000)
        printf("Error! STRTAB_BASE_LO\n");

    /* 11. SMMU_STRTAB_BASE_HI @ 0x84 = 0 */
    mmio_write_32(base + 0x84, 0x00000000);
    if (mmio_read_32(base + 0x84) != 0x00000000)
        printf("Error! STRTAB_BASE_HI\n");

    // /* 12. Clear memory 0x81004000 ~ 0x81006000 */
	// uintptr_t strtab_base = 0x81004000;
	// for (uintptr_t addr = strtab_base; addr < (strtab_base + 0x2000); addr += 4)
	// 	mmio_write_32(addr, 0x00000000);



    // /* 9-B. SMMU_S_STRTAB_BASE_CFG @ 0x8088 = 0x00000006 */
    // mmio_write_32(base + 0x8088, 0x00000006);
    // if (mmio_read_32(base + 0x8088) != 0x00000006)
    //     printf("Error! STRTAB_BASE_CFG\n");

    // /* 10-B. SMMU_S_STRTAB_BASE_LO @ 0x8080 = 0x81004000 */
    // mmio_write_32(base + 0x8080, 0x81004000);
    // if (mmio_read_32(base + 0x8080) != 0x81004000)
    //     printf("Error! STRTAB_BASE_LO\n");

    // /* 11-B. SMMU_S_STRTAB_BASE_HI @ 0x8084 = 0 */
    // mmio_write_32(base + 0x8084, 0x00000000);
    // if (mmio_read_32(base + 0x8084) != 0x00000000)
    //     printf("Error! STRTAB_BASE_HI\n");

    // /* 12-B. Clear memory 0x81004000 ~ 0x81006000 */
	// uintptr_t strtab_base = 0x81004000;
	// for (uintptr_t addr = strtab_base; addr < (strtab_base + 0x2000); addr += 4)
	// 	mmio_write_32(addr, 0x00000000);

    /* 13. DSB */
    dsbsy();

    /* ====================== CMDQ_Init ====================== */

    // /* 14. SMMU_S_CR0 @ 0x8020 : set CMDQEN (bit3) = 1 → 0x00000008 */
    // mmio_write_32(base + 0x8020, 0x00000008);

    // /* 15. Poll SMMU_S_CR0ACK @ 0x8024 waiting bit3 = 1 */
    // smmu_poll(base + 0x8024, (1 << 3), (1 << 3));

    /* 14-B. SMMU_CR0 @ 0x20 : set CMDQEN (bit3) = 1 → 0x00000008 */
    mmio_write_32(base + 0x20, 0x00000008);

    /* 15-B. Poll SMMU_CR0ACK @ 0x24 waiting bit3 = 1 */
    smmu_poll(base + 0x24, (1 << 3), (1 << 3));

    /* ====================== EventQ_Init ====================== */

    // /* 16. SMMU_S_CR0 @ 0x8020 : CMDQEN(8) + EVENTQEN(4) = 0x0000000C */
    // mmio_write_32(base + 0x8020, 0x0000000C);

    // /* 17. Poll SMMU_S_CR0ACK @ 0x8024 waiting bit2 = 1 */
    // smmu_poll(base + 0x8024, (1 << 2), (1 << 2));


    /* 16-B. SMMU_CR0 @ 0x20 : CMDQEN(8) + EVENTQEN(4) = 0x0000000C */
    mmio_write_32(base + 0x20, 0x0000000C);

    /* 17-B. Poll SMMU_CR0ACK @ 0x24 waiting bit2 = 1 */
    smmu_poll(base + 0x24, (1 << 2), (1 << 2));


    /* SMMU_S_Init @ 0x803C waiting bit0 = 1 */
    mmio_write_32(base + 0x803C, 0x00000001);


    /* Poll SMMU_S_Init @ 0x803C waiting bit0 = 0 */
    smmu_poll(base + 0x803C, 1, 0);


	// mmio_write_64(0x81006000, 0x00000200C0003520); // CD Word 0
	// mmio_write_64(0x81006008, 0x0000000081800000); // CD Word 1 (TTB0)
	// //mmio_write_64(0x81006010, 0x0000000000000000); // CD Word 1 (TTB0)
	// // mmio_write_64(0x81006018, 0x0000000000440400); // CD Word 3 (MAIR)
	// mmio_write_64(0x81006018, 0x000000000444ff00); // CD Word 3 (MAIR)

	mmio_write_32(0x81006000, 0xC0003520); // CD Word 0-1
	mmio_write_32(0x81006004, 0x00000200); // CD Word 0-2

	mmio_write_32(0x81006008, 0x81800000); // CD Word 1-1 (TTB0)
	mmio_write_32(0x8100600c, 0x00000000); // CD Word 1-2 (TTB0)

	mmio_write_32(0x81006018, 0x0444ff00); // CD Word 3-1 (MAIR)
	mmio_write_32(0x8100601c, 0x00000000); // CD Word 3-2 (MAIR)


	dsbsy();

	mmio_write_64(0x81004000, 0x81006009); // STE Word： CD Addr，Config-100，V-1（0b1001=0x9）[Stage1 bypass + Stage2 bypass]
	// mmio_write_64(0x81004000, 0x8100600B); // STE Word： CD Addr，Config-101，V-1（0b1011=0xb）[Stage1 trans + Stage2 bypass]

	//[Stage1 bypass + Stage2 trans]
	// mmio_write_64(0x81004000, 0xD); // STE Word 0： Config-110，V-1（0b1101=0xd）
	// mmio_write_64(0x81004008, 0x0); // STE Word 1： 0
	// mmio_write_64(0x81004010, 0x0008002000000001); // STE Word 2
	// mmio_write_64(0x81004018, 0x0000000081800000); // STE Word 3： TTB

	// mmio_write_32(0x81004000, 0x0000000D); // STE Word 0-1： Config-110，V-1（0b1101=0xd）
	// mmio_write_32(0x81004004, 0x0); // STE Word 0-2： Config-110，V-1（0b1101=0xd）

	// mmio_write_32(0x81004008, 0x0); // STE Word 1-1： 0
	// mmio_write_32(0x8100400c, 0x0); // STE Word 1-2： 0

	// mmio_write_32(0x81004010, 0x00000001); // STE Word 2-1
	// mmio_write_32(0x81004014, 0x00080020); // STE Word 2-2

	// mmio_write_64(0x81004018, 0x0000000081800000); // STE Word 3： TEST
	// mmio_read_64(0x81004010); // STE Word 3： TEST
	// mmio_read_64(0x81004018); // STE Word 3： TEST
	// mmio_read_32(0x81004010); // STE Word 3： TEST
	// mmio_read_32(0x81004018); // STE Word 3： TEST
	// mmio_read_32(0x8100401c); // STE Word 3： TEST


	// mmio_write_32(0x81004018, 0x81800000); // STE Word 3-1： TTB
	// mmio_write_32(0x8100401c, 0x0); // STE Word 3-2： 0
	// mmio_write_32(0x81004020, 0x0); // STE Word 4-1： 0
	// mmio_write_32(0x81004024, 0x0); // STE Word 4-2： 0
	// mmio_write_32(0x81004028, 0x0); // STE Word 5-1： 0
	// mmio_write_32(0x8100402c, 0x0); // STE Word 5-2： 0




	
	dsbsy();


    // /* ====================== MSI Config ====================== */
    // mmio_write_32(base + 0x80B0, 0x81100060); // SMMU_S_EVENTQ_IRQ_CFG0
    // mmio_write_32(base + 0x80B8, 0xAAAAAAAA); // SMMU_S_EVENTQ_IRQ_CFG1
    // mmio_write_32(base + 0x80BC, 0x00000021); // SMMU_S_EVENTQ_IRQ_CFG2
    // mmio_write_32(base + 0x8050, 0x00000005); // SMMU_S_IRQ_CTRL
    // smmu_poll(base + 0x8054, 0x5, 0x5); // SMMU_S_IRQ_CTRLACK

	// dsbsy();

    // /* ====================== MSI Config (NS)====================== */
    // mmio_write_32(base + 0x68, 0x81100060); // SMMU_GERROR_IRQ_CFG0
    // mmio_write_32(base + 0xB0, 0x81100060); // SMMU_EVENTQ_IRQ_CFG0
    // mmio_write_32(base + 0xD0, 0x81100060); // SMMU_PRIQ_IRQ_CFG0

    // mmio_write_32(base + 0x70, 0x11111111); // SMMU_GERROR_IRQ_CFG1
    // mmio_write_32(base + 0xB8, 0x22222222); // SMMU_EVENTQ_IRQ_CFG1
    // mmio_write_32(base + 0xD8, 0x33333333); // SMMU_PRIQ_IRQ_CFG1


    // mmio_write_32(base + 0x74, 0x1); // SMMU_GERROR_IRQ_CFG2
    // mmio_write_32(base + 0xBC, 0x00000021); // SMMU_EVENTQ_IRQ_CFG2
 
    // mmio_write_32(base + 0x50, 0x00000007); // SMMU_IRQ_CTRL
    // smmu_poll(base + 0x54, 0x7, 0x7); // SMMU_IRQ_CTRLACK
    // // mmio_write_32(base + 0x50, 0x00000005); // SMMU_IRQ_CTRL
    // // smmu_poll(base + 0x54, 0x5, 0x5); // SMMU_IRQ_CTRLACK

	// dsbsy();


    // uint32_t tmpCMDQ_P=mmio_read_32(base + 0x98); // SMMU_CMDQ_PROD

    // // mmio_write_32(0X81000000, 0x00001046); // Word0
    // // mmio_write_32(0X81000004, 0x00000001); // Word1
    // // mmio_write_32(0X81000008, 0x81100060); // Word2
    // // mmio_write_32(0X8100000c, 0x21000000); // Word3

    // mmio_write_32(0X81000000, 0x00001046); // Word0
    // mmio_write_32(0X81000004, 0x81100060); // Word1
    // mmio_write_32(0X81000008, 0x81100060); // Word2
    // mmio_write_32(0X8100000c, 0x00000000); // Word3

	// dsbsy();
    // mmio_write_32(base + 0x98, (tmpCMDQ_P+1)); // SMMU_CMDQ_PROD
    // /* ====================== MSI Config (NS) END====================== */










    /* ====================== Enable SMMU ====================== */

    // /* 16. SMMU_CR0 @ 0x20 = 1 */
    // mmio_write_32(base + 0x20, 0x00000001);

    // /* 17. Poll SMMU_CR0ACK @ 0x24 waiting bit0 = 1 */
    // smmu_poll(base + 0x24, (1 << 0), (1 << 0));

    /* 16-B. SMMU_CR0 @ 0x20 = 0b1101 = 0xD */
    mmio_write_32(base + 0x20, 0x0000000D);

    /* 17-B. Poll SMMU_CR0ACK @ 0x24 waiting bit0 = 1 */
    smmu_poll(base + 0x24, 0xD, 0xD);


    // /* 16-C. SMMU_S_CR0 @ 0x8020 = 0b1101 = 0xD */
    // mmio_write_32(base + 0x8020, 0x0000000D);

    // /* 17-C. Poll SMMU_S_CR0ACK @ 0x8024 waiting bit0 = 1 */
    // smmu_poll(base + 0x8024, 0xD, 0xD);

    //printf("SMMU register init done.\n");
}



void atf_reg_test_smmu03_GP(void)
{
    uint32_t base = 0x0d000000;
    mmio_write_32(base + 0x44, 0x80001000); // SMMU_GBPA
    smmu_poll(base + 0x44, 0x80000000, 0x0); // SMMU_GBPA	
}

void atf_reg_test_inSoft02(void)
{
	// atf_reg_test_inSoft();
	atf_reg_test_smmu02();
	// atf_reg_test_smmu03_GP();
	// /*===================================================================*/
	// //VA
	// for(uint32_t i=0x81100000;i<0x81100040;i+=0x4)
	// {
	// 	mmio_write_32(i,i);
	// }

	// // // PA
	// // for(uint32_t i=0xD1100000;i<0xD1100040;i+=0x4)
	// // {
	// // 	mmio_write_32(i,0x12345678);
	// // }

	// // for(uint32_t i=0xe0000000;i<0xe0000080;i+=0x8)
	// // {
	// // 	printf("%x  ",mmio_read_32(i));
	// // 	if((i-0x8)%0x10==0)
	// // 	{
	// // 		printf("\n");
	// // 	}
	// // }

	// mmio_write_32(0x40000000, 0x81100000);
	// // if(mmio_read_32(0x40000000)!=0xe0000000)
	// // {
	// // 	printf("Error!pcie-1");
	// // }
	// mmio_write_32(0x40000010, 0x00000405);

	// // mmio_write_32(0x40000010, 0x00400005);
	// // if(mmio_read_32(0x40000010)!=0x00400005)
	// // {
	// // 	printf("Error!pcie-2");
	// // }

	// smmu_poll(0x40000010,(1<<3),(1<<3));
	// //printf("Start to delay (about 90s)\n");
	// // atf_pcie_test();
	// //printf("END delay\n");

	// mmio_write_32(0x40000000, 0x81100040);
	// // if(mmio_read_32(0x40000000)!=0xe0000040)
	// // {
	// // 	printf("Error!pcie-3");
	// // }
	// mmio_write_32(0x40000010, 0x00000403);

	// // mmio_write_32(0x40000010, 0x00400003);
	// // if(mmio_read_32(0x40000010)!=0x00400003)
	// // {
	// // 	printf("Error!pcie-4");
	// // }
	// smmu_poll(0x40000010,(1<<3),(1<<3));

	// // atf_pcie_test();

	// // for(uint32_t i=0xe0000000;i<0xe0000080;i+=0x8)
	// // {
	// // 	printf("%x  ",mmio_read_32(i));
	// // 	if((i-0x8)%0x10==0)
	// // 	{
	// // 		printf("\n");
	// // 	}
	// // }

	// mmio_write_32(0x40000000, 0x81100040);

	// mmio_write_32(0x40000010, 0x00000405);

	// smmu_poll(0x40000010,(1<<3),(1<<3));
	// // /*======================== Deal with 0x4000 ========================*/

	// for(uint32_t i=0x81100000;i<0x81104000;i+=0x4)
	// {
	// 	mmio_write_32(i,i);
	// }

	// // PA
	// for(uint32_t i=0xD1100000;i<0xD1100040;i+=0x4)
	// {
	// 	mmio_write_32(i,i);
	// }

	// mmio_write_32(0x40000000, 0x81100000);

	// mmio_write_32(0x40000010, 0x00004005);

	// smmu_poll(0x40000010,(1<<3),(1<<3));

	// mmio_write_32(0x40000000, 0x81100400);

	// mmio_write_32(0x40000010, 0x00004003);

	// smmu_poll(0x40000010,(1<<3),(1<<3));

	// mmio_write_32(0x40000000, 0x81100400);

	// mmio_write_32(0x40000010, 0x00004005);

	// smmu_poll(0x40000010,(1<<3),(1<<3));
	// /**======================== Deal with 0x4000 END========================*/


	// /*======================== write CMD by pcie ========================*/
	//smmu寄存器操作
	uint32_t base = 0x0d000000;


	//往暂用空间放命令，0X81100000~0X8110000c
    mmio_write_32(0X81100000, 0x00001046); // Word0
    mmio_write_32(0X81100004, 0x81100060); // Word1
    mmio_write_32(0X81100008, 0x81100060); // Word2
    mmio_write_32(0X8110000c, 0x00000000); // Word3

	//操作pcie，在0x81100000读取0x100字节的空间，保证覆盖前述命令
	mmio_write_32(0x40000000, 0x81100000);
	mmio_write_32(0x40000010, 0x00000015);
	smmu_poll(0x40000010,(1<<3),(1<<3));

	//操作pcie，往目标地址0x81000000【CMDQ】写0x100字节刚才读到的内容
	mmio_write_32(0x40000000, 0x81000000);
	mmio_write_32(0x40000010, 0x00000013);
	smmu_poll(0x40000010,(1<<3),(1<<3));

	//操作pcie，在目标地址读0x100字节，确认情况
	mmio_write_32(0x40000000, 0x81000000);
	mmio_write_32(0x40000010, 0x00000015);
	smmu_poll(0x40000010,(1<<3),(1<<3));

	uint32_t tmpCMDQ_P=mmio_read_32(base + 0x98); // SMMU_CMDQ_PROD
	//smmu寄存器操作
	dsbsy();
    mmio_write_32(base + 0x98, (tmpCMDQ_P+1)); // SMMU_CMDQ_PROD

	/**======================== Deal with 0x4000 END========================*/

}

void atf_reg_test_inSoft03(void)
{
	// atf_reg_test_inSoft();
	// atf_reg_test_smmu02();
	atf_reg_test_smmu03_GP();

    mmio_write_32(0x81100000, 0x81100060); 

	//操作pcie，在0x81100000读取0x10字节的空间，保证覆盖前述命令
	mmio_write_32(0x40000000, 0x81100000);
	mmio_write_32(0x40000010, 0x00000015);
	smmu_poll(0x40000010,(1<<3),(1<<3));


	mmio_write_32(0x40000000, 0x81100060);
	mmio_write_32(0x40000010, 0x00000013);
	smmu_poll(0x40000010,(1<<3),(1<<3));

	//在目标地址读0x10字节，确认情况
	mmio_write_32(0x40000000, 0x81100060);
	mmio_write_32(0x40000010, 0x00000015);
	smmu_poll(0x40000010,(1<<3),(1<<3));

	volatile uint32_t tmp=0;
	tmp=mmio_read_32(0x81100060);
	tmp+=1;

}

void atf_reg_test_inSoft04(void)
{
	atf_reg_test_smmu03_GP();

	for(uint32_t i=0x81100000;i<0x81100ff0;i+=0x4)
	{
		mmio_write_32(i,i);
	}

	//操作pcie，在0x81100000读取0x10字节的空间，保证覆盖前述命令
	mmio_write_32(0x40000000, 0x81100000);
	mmio_write_32(0x40000010, 0x00000ff5);
	smmu_poll(0x40000010,(1<<3),(1<<3));


	mmio_write_32(0x40000000, 0x81100060);
	mmio_write_32(0x40000010, 0x00000ff3);
	smmu_poll(0x40000010,(1<<3),(1<<3));

	//在目标地址读0x10字节，确认情况
	mmio_write_32(0x40000000, 0x81100060);
	mmio_write_32(0x40000010, 0x00000ff5);
	smmu_poll(0x40000010,(1<<3),(1<<3));

}


void atf_alignment_test(void)
{
	mmio_write_64(0x81300000, 0x1230456789abcdef);
	mmio_write_64(0x81300010, 0x1230456789abcdef);
	mmio_write_64(0x81300020, 0x1230456789abcdef);
	mmio_write_64(0x81300028, 0x1230456789abcdef);
	mmio_write_64(0x81300030, 0x1230456789abcdef);
	mmio_write_64(0x81300048, 0x1230456789abcdef);

	mmio_read_64(0x81300000);
	mmio_read_64(0x81300008);
	mmio_read_64(0x81300010);
	mmio_read_64(0x81300018);
	mmio_read_64(0x81300020);
	mmio_read_64(0x81300028);
	mmio_read_64(0x81300030);
	mmio_read_64(0x81300038);
	mmio_read_64(0x81300040);
	mmio_read_64(0x81300048);

}

void atf_scan_DDR(uint32_t sta, uint32_t en)
{
    uint32_t pattern1=0xAA55AA55;
    uint32_t pattern2=0x55AA55AA;
    uint32_t bef=0x0,aft=0x0;

    for(uint32_t i = sta;i<en;i+=0x4)
    {
        bef=mmio_read_32(i);
        (void)bef;
        mmio_write_32(i,pattern1);
        aft=mmio_read_32(i);
        if(aft!=pattern1)
        {
            // printf("Mismatch(p1) in 0x%x - 0x%x,before is 0x%x\n", i, i+4,bef);
            while(1) { /* 死循环 */ }
        }

        mmio_write_32(i,pattern2);
        aft=mmio_read_32(i);
        if(aft!=pattern2)
        {
            // printf("Mismatch(p2) in 0x%x - 0x%x\n", i, i+4);
            while(1) { /* 死循环 */ }
        }

    }
}

void atf_pcie_test2()
{
    uint32_t base=0x40000000;
	volatile uint32_t tmp=0;

    tmp=mmio_read_32(base);
    tmp=mmio_read_32(base + 0x144);
    tmp=mmio_read_32(base + 0x13C);

	mmio_write_32(base + 0x13C, 0x100);

    tmp=mmio_read_32(base + 0x13C);

    tmp=mmio_read_32(base + 0x148);
	mmio_write_32(base + 0x148, 0xffffffff);
    tmp=mmio_read_32(base + 0x148);

    tmp=mmio_read_32(0x60000000);
	mmio_write_32(0x60000000, 0x12345678);
    tmp=mmio_read_32(0x60000000);

    tmp=mmio_read_32(0x60000004);
	mmio_write_32(0x60000004, 0x12345678);
    tmp=mmio_read_32(0x60000004);

    tmp=mmio_read_32(0x70000000);
	mmio_write_32(0x70000000, 0x12345678);
    tmp=mmio_read_32(0x70000000);

    tmp=mmio_read_32(0x70000004);
	mmio_write_32(0x70000004, 0x12345678);
    tmp=mmio_read_32(0x70000004);

	tmp+=1;
}

void bl1_setup(void)
{
	/* Enable early console if EARLY_CONSOLE flag is enabled */

	//【改】测试新设计的一些寄存器以及sram
	//atf_reg_test_inSoft();

	//atf_alignment_test();
	//atf_scan_DDR(0x81000000,0xffffffff);
	// atf_reg_test_inSoft02();
	// atf_pcie_test2();
	// atf_reg_test_inSoft03();
    //atf_reg_test_inSoft04();
	// atf_scan_DDR(0x80000000,0x81000000);

	plat_setup_early_console();



	/* Perform early platform-specific setup */
	bl1_early_platform_setup();
	//atf_testpoint_print_RegGroup3();
	// atf_testpoint_printMEM_32B(0x40000000);
	// ATF_BP(" Read 0x40000000 in bl1_setup is 0x%lx[bl1_main]\n",mmio_read_64(0x40000000));
	// assert((mmio_read_64(0x40000000)) != 0);
	// assert((void*)(mmio_read_64(0x40000000)) != NULL);



	/* Perform late platform-specific setup */
	ATF_BP(" START: bl1_plat_arch_setup(); in bl1_setup [bl1_main]\n");
	bl1_plat_arch_setup();
	ATF_BP(" FINISH: bl1_plat_arch_setup(); in bl1_setup [bl1_main]\n");
	//atf_testpoint_print_RegGroup3();

	// atf_reg_test();

}

/*******************************************************************************
 * Function to perform late architectural and platform specific initialization.
 * It also queries the platform to load and run next BL image. Only called
 * by the primary cpu after a cold boot.
 ******************************************************************************/


















void bl1_main(void)
{
	unsigned int image_id;

#if ENABLE_RUNTIME_INSTRUMENTATION
	PMF_CAPTURE_TIMESTAMP(bl_svc, BL1_ENTRY, PMF_CACHE_MAINT);
#endif

	//printf("I am ATF\n");
	// // dump_fip_bin_contents(BL1bin_ADDR);
	// // dump_fip_bin_contents(FIP_ADDR);
	// dump_fip_bin_contents();
	// // write_fip_bin_contents();
	// // dump_fip_bin_contents();
	// dump_fip_bin_contents1();
	// dump_fip_bin_contents2();
	// dump_fip_bin_contents3();
	// write_fip_bin_contents3();
	// dump_fip_bin_contents3();

	/* Announce our arrival */
	//NOTICE(FIRMWARE_WELCOME_STR);
	//NOTICE("BL1: %s\n", build_version_string);
	//NOTICE("BL1: %s\n", build_message);

	INFO("BL1: RAM %p - %p\n", (void *)BL1_RAM_BASE, (void *)BL1_RAM_LIMIT);
	

	print_errata_status();

	// atf_reg_test();
	//atf_reg_test_smmu02();
	// atf_reg_test_inSoft02();

	// atf_scan_Memory_tmpAll();
	//atf_scan_Memory_fast_mode();


	// //测试该地址[32]
	// ATF_BP(" Read 0x40000000 in bl1_setup is 0x%x[bl1_main]\n",mmio_read_32(0x40000000));
	// assert((mmio_read_32(0x40000000)) != 0);

	// //测试该地址
	// ATF_BP(" Read 0x40000000 in bl1_setup is 0x%lx[bl1_main]\n",mmio_read_64(0x40000000));
	// assert((mmio_read_64(0x40000000)) != 0);
	// assert((void*)(mmio_read_64(0x40000000)) != NULL);
	// atf_scan_Memory_fast_mode();

	// printf("Start to delay\n");
	// delay_busywait(9e6/4);
	// printf("END delay\n");
	// atf_scan_Memory_fast_mode();
	// printf(" Read 0xf7990000 in bl1_setup is 0x%lx[bl1_main]\n",mmio_read_64(0xf7990000));
	// printf(" Read 0x40000000 in bl1_setup is 0x%lx[bl1_main]\n",mmio_read_64(0x40000000));
	// printf(" Read 0x40000144 in bl1_setup is 0x%lx[bl1_main]\n",mmio_read_64(0x40000144));
	// atf_pcie_test();
	// atf_scan_Memory_fast_mode();
	// atf_scan_Memory_miniConfirm();
	// print_ulong_hex(mmio_read_64(0xf7990000));

#if ENABLE_ASSERTIONS
	u_register_t val;
	/*
	 * Ensure that MMU/Caches and coherency are turned on
	 */
#ifdef __aarch64__
	val = read_sctlr_el3();
#else
	val = read_sctlr();
#endif
	// assert((val & SCTLR_M_BIT) != 0);
	assert((val & SCTLR_C_BIT) != 0);
	assert((val & SCTLR_I_BIT) != 0);
	/*
	 * Check that Cache Writeback Granule (CWG) in CTR_EL0 matches the
	 * provided platform value
	 */
	val = (read_ctr_el0() >> CTR_CWG_SHIFT) & CTR_CWG_MASK;
	/*
	 * If CWG is zero, then no CWG information is available but we can
	 * at least check the platform value is less than the architectural
	 * maximum.
	 */
	if (val != 0)
		assert(CACHE_WRITEBACK_GRANULE == SIZE_FROM_LOG2_WORDS(val));
	else
		assert(CACHE_WRITEBACK_GRANULE <= MAX_CACHE_LINE_SIZE);
#endif /* ENABLE_ASSERTIONS */

	/* Perform remaining generic architectural setup from EL3 */
	bl1_arch_setup();
	//atf_testpoint_print_RegGroup3();

	crypto_mod_init();

	/* Initialize authentication module */
	auth_mod_init();

	/* Initialize the measured boot */
	bl1_plat_mboot_init();
	//atf_testpoint_print_RegGroup3();

	/* Perform platform setup in BL1. */
	ATF_BP(" START: bl1_platform_setup in bl1_main [bl1_main]\n");
	bl1_platform_setup();
	ATF_BP(" FINISH: bl1_platform_setup in bl1_main [bl1_main]\n");

	/* Get the image id of next image to load and run. */
	ATF_BP(" START: bl1_plat_get_next_image_id in bl1_main [bl1_main]\n");
	image_id = bl1_plat_get_next_image_id();
	ATF_BP(" FINISH: bl1_plat_get_next_image_id in bl1_main [bl1_main]\n");

	/*
	 * We currently interpret any image id other than
	 * BL2_IMAGE_ID as the start of firmware update.
	 */
	ATF_BP(" START: bl1_load_bl2 in bl1_main [bl1_main]\n");
	
	if (image_id == BL2_IMAGE_ID)
		bl1_load_bl2();
	else
		NOTICE("BL1-FWU: *******FWU Process Started*******\n");

	ATF_BP(" FINISH: bl1_load_bl2 in bl1_main [bl1_main]\n");


	ATF_BP(" START: bl1_plat_mboot_finish in bl1_main [bl1_main]\n");

	/* Teardown the measured boot driver */
	bl1_plat_mboot_finish();
	//atf_testpoint_print_RegGroup3();

	ATF_BP(" FINISH: bl1_plat_mboot_finish in bl1_main [bl1_main]\n");

	ATF_BP(" START: crypto_mod_finish in bl1_main [bl1_main]\n");
	crypto_mod_finish();
	ATF_BP(" FINISH: crypto_mod_finish in bl1_main [bl1_main]\n");
	//atf_testpoint_print_RegGroup3();


	ATF_BP(" START: bl1_prepare_next_image in bl1_main [bl1_main]\n");
	bl1_prepare_next_image(image_id);
	ATF_BP(" FINISH: bl1_prepare_next_image in bl1_main [bl1_main]\n");
	//atf_testpoint_print_RegGroup3();



#if ENABLE_RUNTIME_INSTRUMENTATION
	PMF_CAPTURE_TIMESTAMP(bl_svc, BL1_EXIT, PMF_CACHE_MAINT);
#endif

	ATF_BP(" START: console_flush in bl1_main [bl1_main]\n");
	console_flush();
	ATF_BP(" FINISH: console_flush in bl1_main [bl1_main]\n");

	ATF_BP(" Here we finish bl1_main [bl1_main]\n");
	//atf_testpoint_print_RegGroup3();
	printf("ENABLE_PAUTH is %d\n", ENABLE_PAUTH);
	printf("ENABLE_RME is %d\n", ENABLE_RME);

}

/*******************************************************************************
 * This function locates and loads the BL2 raw binary image in the trusted SRAM.
 * Called by the primary cpu after a cold boot.
 * TODO: Add support for alternative image load mechanism e.g using virtio/elf
 * loader etc.
 ******************************************************************************/
static void bl1_load_bl2(void)
{
	image_desc_t *desc;
	image_info_t *info;
	int err;

	/* Get the image descriptor */
	desc = bl1_plat_get_image_desc(BL2_IMAGE_ID);
	assert(desc != NULL);

	/* Get the image info */
	info = &desc->image_info;
	INFO("BL1: Loading BL2\n");

	err = bl1_plat_handle_pre_image_load(BL2_IMAGE_ID);
	if (err != 0) {
		ERROR("Failure in pre image load handling of BL2 (%d)\n", err);
		plat_error_handler(err);
	}

	err = load_auth_image(BL2_IMAGE_ID, info);
	if (err != 0) {
		ERROR("Failed to load BL2 firmware.\n");
		plat_error_handler(err);
	}

	/* Allow platform to handle image information. */
	err = bl1_plat_handle_post_image_load(BL2_IMAGE_ID);
	if (err != 0) {
		ERROR("Failure in post image load handling of BL2 (%d)\n", err);
		plat_error_handler(err);
	}

	NOTICE("BL1: Booting BL2\n");
}

/*******************************************************************************
 * Function called just before handing over to the next BL to inform the user
 * about the boot progress. In debug mode, also print details about the BL
 * image's execution context.
 ******************************************************************************/
void bl1_print_next_bl_ep_info(const entry_point_info_t *bl_ep_info)
{
	////atf_testpoint_print_RegGroup3();

#ifdef __aarch64__
	NOTICE("BL1: Booting BL31\n");
#else
	NOTICE("BL1: Booting BL32\n");
#endif /* __aarch64__ */
	print_entry_point_info(bl_ep_info);
	////atf_testpoint_print_RegGroup3();

}
void bl1_print_next_in_SynchronousExceptionA64()
{
	ATF_BP(" Here is in SynchronousExceptionA64\n");
	//atf_testpoint_print_RegGroup3();
}
void bl1_print_next_in_smc_handler64()
{
	ATF_BP(" Here is in smc_handler64\n");
	//atf_testpoint_print_RegGroup3();
}
void bl1_print_next_in_smc_handler()
{
	ATF_BP(" Here is in smc_handler\n");
	//atf_testpoint_print_RegGroup3();
}
/*
void bl1_print_Reg_in_SynchronousExceptionA64()
{
	unsigned long sp_el3, vbar, scr;
	unsigned long *next = (unsigned long *)next_bl_ep_info;

	asm volatile("mrs %0, SP_EL3"  : "=r"(sp_el3));
	asm volatile("mrs %0, VBAR_EL3": "=r"(vbar));
	asm volatile("mrs %0, SCR_EL3" : "=r"(scr));

	ATF_BP(" PRE-SMC: next=0x%lx entry=0x%lx spsr=0x%lx SP_EL3=0x%lx VBAR=0x%lx SCR=0x%lx\n",
		(unsigned long)next_bl_ep_info, next ? next[0] : 0, next ? next[1] : 0,
		sp_el3, vbar, scr);
}
*/
#if SPIN_ON_BL1_EXIT
void print_debug_loop_message(void)
{
	NOTICE("BL1: Debug loop, spinning forever\n");
	NOTICE("BL1: Please connect the debugger to continue\n");
}
#endif

/*******************************************************************************
 * Top level handler for servicing BL1 SMCs.
 ******************************************************************************/
u_register_t bl1_smc_handler(unsigned int smc_fid,
	u_register_t x1,
	u_register_t x2,
	u_register_t x3,
	u_register_t x4,
	void *cookie,
	void *handle,
	unsigned int flags)
{
	/* BL1 Service UUID */
	DEFINE_SVC_UUID2(bl1_svc_uid,
		U(0xd46739fd), 0xcb72, 0x9a4d, 0xb5, 0x75,
		0x67, 0x15, 0xd6, 0xf4, 0xbb, 0x4a);


#if TRUSTED_BOARD_BOOT
	/*
	 * Dispatch FWU calls to FWU SMC handler and return its return
	 * value
	 */
	if (is_fwu_fid(smc_fid)) {
		return bl1_fwu_smc_handler(smc_fid, x1, x2, x3, x4, cookie,
			handle, flags);
	}
#endif

	switch (smc_fid) {
	case BL1_SMC_CALL_COUNT:
		SMC_RET1(handle, BL1_NUM_SMC_CALLS);

	case BL1_SMC_UID:
		SMC_UUID_RET(handle, bl1_svc_uid);

	case BL1_SMC_VERSION:
		SMC_RET1(handle, BL1_SMC_MAJOR_VER | BL1_SMC_MINOR_VER);

	default:
		WARN("Unimplemented BL1 SMC Call: 0x%x\n", smc_fid);
		SMC_RET1(handle, SMC_UNK);
	}
}

/*******************************************************************************
 * BL1 SMC wrapper.  This function is only used in AArch32 mode to ensure ABI
 * compliance when invoking bl1_smc_handler.
 ******************************************************************************/
u_register_t bl1_smc_wrapper(uint32_t smc_fid,
	void *cookie,
	void *handle,
	unsigned int flags)
{
	u_register_t x1, x2, x3, x4;

	assert(handle != NULL);

	get_smc_params_from_ctx(handle, x1, x2, x3, x4);
	return bl1_smc_handler(smc_fid, x1, x2, x3, x4, cookie, handle, flags);
}










