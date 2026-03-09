/* 
后期代替platform_def.h
地址来源：SoC地址空间分配.docx
 */

#ifndef CX_PLATFORM_DEF_H
#define CX_PLATFORM_DEF_H

// 1. QSPI Flash（引导 + 剩余空间）【QSPI （HN-D之NIC）】
#define CX_QSPI_FLASH_BASE           UL(0x00000000)
#define CX_QSPI_FLASH_SIZE           UL(0x08000000)  /* 128MB */
/* 内部需要继续划分功能区 */


// 2. LPC （HN-D之NIC）
#define CX_LPC_BASE                  UL(0x08000000)
#define CX_LPC_SIZE                  UL(0x04000000)  /* 64MB */


// 3. 外设【除RN-F、SN-F外的设备寄存器地址，除表中单独列出的寄存器以外，其它所有寄存器 （HN-D之NIC）】
#define CX_DEVICES_GROUP0_BASE               UL(0x0C000000)
#define CX_DEVICES_GROUP0_SIZE               UL(0x01000000)  /* 16MB */

// 3.1 SoC设备地址空间【SoC系统设备基地址0x000_0C00_0000】
#define CX_UART0_BASE                UL(0x0C000000)
#define CX_UART0_SIZE                UL(0x1000)  /* 4KB */
#define CX_UART1_BASE                UL(0x0C001000)
#define CX_UART1_SIZE                UL(0x1000)  /* 4KB */
//---- 留空 8KB ----
#define CX_GPIO0_BASE                UL(0x0C004000)
#define CX_GPIO0_SIZE                UL(0x1000)  /* 4KB */
#define CX_GPIO1_BASE                UL(0x0C005000)
#define CX_GPIO1_SIZE                UL(0x1000)  /* 4KB */

#define CX_I2C0_BASE                 UL(0x0C006000)
#define CX_I2C0_SIZE                UL(0x1000)  /* 4KB */
#define CX_I2C1_BASE                 UL(0x0C007000)
#define CX_I2C1_SIZE                UL(0x1000)  /* 4KB */
#define CX_I2C2_BASE                 UL(0x0C008000)
#define CX_I2C2_SIZE                UL(0x1000)  /* 4KB */
#define CX_I3C_BASE                  UL(0x0C009000)
#define CX_I3C_SIZE                UL(0x1000)  /* 4KB */

#define CX_WDT_BASE                  UL(0x0C00A000)
#define CX_WDT_SIZE                UL(0x2000)  /* 8KB */
#define CX_SPI_BASE                  UL(0x0C00C000)
#define CX_SPI_SIZE                UL(0x1000)  /* 4KB */
#define CX_TIMER_BASE                UL(0x0C00D000) /* 用于 system counter 替代？ */
#define CX_TIMER_SIZE                UL(0x1000)  /* 4KB */
//---- 留空 8KB ----
#define CX_QSPI_CTRL_BASE            UL(0x0C014000)
#define CX_QSPI_CTRL_SIZE                UL(0x1000)  /* 4KB */
#define CX_PCIE_REG_BASE             UL(0x0C017000)
#define CX_PCIE_REG_SIZE                UL(0x2000)  /* 8KB */

// 4. SMMU【SMMU （HN-D之NIC）】
#define CX_SMMU_BASE                 UL(0x0D000000)
#define CX_SMMU_SIZE                 UL(0x01000000)  /* 16MB */

//4.1 SMMU地址空间（分TCU和TBU两段）
#define CX_SMMU_TCU_BASE                 UL(0x0D000000)
#define CX_SMMU_TCU_SIZE                 UL(0x0003FFFD)  /* 255KB */
#define CX_SMMU_TBU_BASE                 UL(0x0D040000)
#define CX_SMMU_TBU_SIZE                 UL(0x0001FFFD)  /* 127KB */


// 5. GIC【GIC中断控制器（HN-I）】
#define CX_GIC_BASE                  UL(0x0E000000)
#define CX_GIC_SIZE                  UL(0x01000000)  /* 16MB */

// 5.1 中断控制器内部地址划分
#define CX_GIC_DIST_BASE             (CX_GIC_BASE + 0x00000000)  /* Distributor 寄存器 */
#define CX_GIC_MSI_SPI_BASE          (CX_GIC_BASE + 0x00010000) /* Distributor 基于消息的SPI */
#define CX_GIC_ITS_CTRL_BASE         (CX_GIC_BASE + 0x00020000) /* ITS 控制寄存器 */
#define CX_GIC_ITS_TRANS_BASE        (CX_GIC_BASE + 0x00030000) /* ITS 地址转换寄存器 */
#define CX_GIC_ITS_VSGI_BASE         (CX_GIC_BASE + 0x00040000) /* ITS vSGI 寄存器 */
#define CX_GIC_GICT_BASE             (CX_GIC_BASE + 0x00050000) /* GICT寄存器 */
#define CX_GIC_GICP_BASE             (CX_GIC_BASE + 0x00060000) /* GICP寄存器 */
#define CX_GIC_VLPI_BASE             (CX_GIC_BASE + 0x00070000) /* VLPI寄存器 */
#define CX_GIC_RD_CTRL_BASE          (CX_GIC_BASE + 0x00080000) /* RD 控制和物理LPI */
#define CX_GIC_RD_SGI_PPI_BASE       (CX_GIC_BASE + 0x00090000) /* RD SGI & PPI */
#define CX_GIC_RSVD10_BASE           (CX_GIC_BASE + 0x000A0000) /* 保留 */
#define CX_GIC_RSVD11_BASE           (CX_GIC_BASE + 0x000B0000) /* 保留 */
#define CX_GIC_RSVD12_BASE           (CX_GIC_BASE + 0x000C0000) /* 保留 */
#define CX_GIC_RSVD13_BASE           (CX_GIC_BASE + 0x000D0000) /* 保留 */
#define CX_GIC_RSVD14_BASE           (CX_GIC_BASE + 0x000E0000) /* 保留 */
#define CX_GIC_RSVD15_BASE           (CX_GIC_BASE + 0x000F0000) /* 保留 */
#define CX_GIC_REG_SIZE             UL(0x00010000)          /* 64KB */


// 6. Sram（64KB）（SBSX）
//SRAM主要用于CPU boot时执行代码用的存储空间，目前设置为64KB。
#define CX_SRAM_BASE                 UL(0x0F000000)
#define CX_SRAM_SIZE                 UL(0x00010000)  /* 64KB */
#define CX_SRAM_KEEP_SIZE                 UL(0x01000000)  /* 16MB */


// 7. DMC_PERIPHASE （SN-F的device：DDR）
#define CX_DMC_PERIPH_BASE           UL(0x10000000)
#define CX_DMC_PERIPH_SIZE           UL(0x10000000)  /* 256MB */


// 8. DSU_PERIPHBASE （RN-F的device：core和NFU）
#define CX_DSU_PERIPH_BASE           UL(0x20000000)
#define CX_DSU_PERIPH_SIZE           UL(0x10000000)


// 9. CFGM_PERIPHBASE片内网络内部寄存器空间
#define CX_CFGM_BASE                 UL(0x30000000)
#define CX_CFGM_SIZE                 UL(0x10000000)


// 10. PCIE的配置、IO和MEM32空间 （HN-P）
#define CX_PCIE32_BASE               UL(0x40000000)
#define CX_PCIE32_SIZE               UL(0x40000000)  /* 1GB */

//【改】类型改成ULL，因为atf代码里到这个级别也是用了ULL而不是UL
// 11. Memory空间
#define CX_DRAM_BASE                 ULL(0x80000000)
#define CX_DRAM_SIZE                 ULL(0x80000000)  /* 2GB */


// 12. PCIE的MEM64空间
#define CX_PCIE64_BASE               UL(0x000100000000)
#define CX_PCIE64_SIZE               UL(0x000800000000)  /* 32GB */


// 13. 0x0009_0000_0000~0x000F_FFFF_FFFF	24GB	保留


// 14. 0x0010_0000_0000~0xFFFF_FFFF_FFFF 	TB	扩展Memory 空间

//串口参数
// #define PLAT_CX_BOOT_UART_CLK_IN_HZ   5700000 //5.7MHZ，输入这些参数的时候需要提前算一下，因为没有开分数分频寄存器，所以小数会被舍弃，这需要注意。
// #define CX_CONSOLE_BAUDRATE           6566 //目标波特率
#define PLAT_CX_BOOT_UART_CLK_IN_HZ   6250000
#define CX_CONSOLE_BAUDRATE           7200 //目标波特率
// #define PLAT_CX_BOOT_UART_CLK_IN_HZ   12500000
// #define CX_CONSOLE_BAUDRATE           14400 //目标波特率

// MMU 参数
//#define CX_MAX_MMAP_REGIONS          16
//#define CX_MAX_XLAT_TABLES           8


// Trusted DRAM
//#define CX_TRUSTED_DRAM_BASE         (CX_DRAM_BASE + UL(0x01000000))  /* DDR+16MB */
//#define CX_TRUSTED_DRAM_SIZE         UL(0x00200000)  /* 2MB */

/*
NFU地址空间，基地址0x000_0F80_0000
NFUs基地址0x000_0F80_0000。
256个NFU（0-255）,每个 4KB
*/
#define CX_NFU_BASE                     UL(0x0F800000)
#define CX_NFU_SIZE                   UL(0x1000)  /* 4KB */
#define CX_NFU_COUNT                    256

// 获取第 i 个 NFU 的基地址
#define CX_NFUn_BASE(i)                 (CX_NFU_BASE + ((i) * CX_NFU_SIZE))  /* i = 0 ~ 255 */

// 结构相关寄存器
#define CX_NFU_REG_NEURON_BASE_OFFSET        0x000  /* NeuronBase */
#define CX_NFU_REG_CONNECTION_BASE_OFFSET    0x008  /* ConnectionBase */
#define CX_NFU_REG_HI_BASE_OFFSET            0x010  /* HiBase */
#define CX_NFU_REG_START_PC_OFFSET           0x018  /* StartPC */

//lastlayerptr_totalneuroncnt0-15
#define CX_NFU_REG_LASTLAYER_PTR_OFFSET(i)   (0x020 + ((i) * 8))  /* i = 0 ~ 15 */

#define CX_NFU_REG_CTRL_MODE_OFFSET          0x0A0  /* decay_mode, sync_mode, RegGncMsk, StartNfu */
#define CX_NFU_REG_DATA_OFFSET               0x0A8  /* RegData */
#define CX_NFU_REG_COMMAND_OFFSET            0x0B0  /* RegBroadCast, RegDst, RegAddr, RegCmd, RegOp */
#define CX_NFU_REG_OP_CMPL_OFFSET            0x0B8  /* RegOpCmpl */

// 任务相关寄存器
#define CX_NFU_REG_TASK_CONFIG_OFFSET        0x100  /* vth, batch, batch_size */
#define CX_NFU_REG_SPIKEIN_PTR_OFFSET        0x108  /* spikeInPtr */
#define CX_NFU_REG_SPIKEOUT_PTR_OFFSET       0x110  /* spikeOutPtr */
#define CX_NFU_REG_NET1STLAYER_PTR_OFFSET    0x118  /* net1stlayerPtr */
#define CX_NFU_REG_TASK_STATUS_OFFSET        0x120  /* idlestep, runstep, netlayers, spikeOutbuflen, finish, start */

//【改】增加Watchdog所需相关参数：CLK_HZ、TIMEOUT_SEC、LOAD_VAL
#define CX_TWDG_CLK_HZ		100*1000*1000
//#define CX_TWDG_CLK_HZ		12.5*1000*1000

//受模式选择与计数器位宽等因素影响，实际超时时间范围设置到 (2 x 16 = 32 sec) 
#define CX_TWDG_TIMEOUT_SEC		16
#define CX_TWDG_LOAD_VAL		(CX_TWDG_CLK_HZ * CX_TWDG_TIMEOUT_SEC)

#endif /* CX_SOC_MAP_H */
