/*
 * Copyright (c) 2014-2025, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef PLATFORM_DEF_H
#define PLATFORM_DEF_H

#include <drivers/arm/tzc400.h>
#include <lib/utils_def.h>
#include <plat/arm/board/common/v2m_def.h>
#include <plat/arm/common/arm_def.h>
#include <plat/arm/common/arm_spm_def.h>
#include <plat/common/common_def.h>

#include "../fvp_def.h"

/*
* 包含 mbedTLS 配置文件，以便在可信启动流程中使用 mbedTLS进行加密操作。
* 对于支持可信板启动 (TBB) 的平台，此功能是必需的。
* mbedTLS 配置文件预计在构建系统中定义并包含在此处。
 TRUSTED_BOARD_BOOT 赋值：
 “:=0”：make_helpers\defaults.mk ；plat\socionext\synquacer\platform.mk
 “:=1”：plat\renesas\common\common.mk ；plat\brcm\board\common\board_common.mk
 “?=0”： plat\st\common\common.mk
 * If TRUSTED_BOARD_BOOT is not defined, the mbedTLS configuration file
 * must not be included.
*/
#if TRUSTED_BOARD_BOOT	// 受信任板级启动
#include MBEDTLS_CONFIG_FILE	//安全启动流程提供加密支持
#endif

/* Required platform porting definitions */
/*
 * 定义平台核心数量= 集群数 * 每个集群的最大 CPU 数 * 每个 CPU 的最大 PE 数
 * 即FVP_CLUSTER_COUNT * FVP_MAX_CPUS_PER_CLUSTER * FVP_MAX_PE_PER_CPU

 \plat\arm\board\fvp\platform.mk：
# Default cluster count for FVP
FVP_CLUSTER_COUNT		:= 2

# Default number of CPUs per cluster on FVP
FVP_MAX_CPUS_PER_CLUSTER	:= 4

# Default number of threads per CPU on FVP
FVP_MAX_PE_PER_CPU		:= 1
*/
#define PLATFORM_CORE_COUNT  (U(FVP_CLUSTER_COUNT) * \
			      U(FVP_MAX_CPUS_PER_CLUSTER) * \
			      U(FVP_MAX_PE_PER_CPU))

/*
 * 定义平台电源域数量= 集群数 + 核心数 + 1
 * 其中，集群数为 FVP_CLUSTER_COUNT，核心数为 PLATFORM_CORE_COUNT
 * 最后加 1 是为了包含一个系统级别的电源域。
 * PSCI 实现使用此宏来分配用于表示电源域拓扑的数据结构
 * 这个定义用于描述平台的电源管理结构。
*/
#define PLAT_NUM_PWR_DOMAINS (U(FVP_CLUSTER_COUNT) + \
			      PLATFORM_CORE_COUNT + U(1))


/* Platform porting definitions for power management */
/*
 * 定义平台最大电源级别= ARM_PWR_LVL2
 * 这是因为 FVP 平台支持三个电源级别：ARM_PWR_LVL0、ARM_PWR_LVL1 和 ARM_PWR_LVL2
 * ARM_PWR_LVL0 对应于 FVP 中的低功耗状态
 * ARM_PWR_LVL1 对应于 FVP 中的 CPU 挂起状态
 * ARM_PWR_LVL2 对应于 FVP 中的正常工作状态
 * 选择 ARM_PWR_LVL2 是因为它提供了最大的能量效率和性能
 * 在 FVP 平台上，这是一个合理的默认值
*/

#define PLAT_MAX_PWR_LVL		ARM_PWR_LVL2

/*
 * 定义平台最大 CPU 挂起电源级别= ARM_PWR_LVL1
 * 在 CPU 挂起时，最大只支持到集群级（Level 1）的挂起。
*/
#if PSCI_OS_INIT_MODE
#define PLAT_MAX_CPU_SUSPEND_PWR_LVL	ARM_PWR_LVL1
#endif

/*
 * Other platform porting definitions are provided by included headers
 其他平台移植相关的宏定义或配置项已经通过包含的头文件（included headers）提供。
 这通常意味着本文件只包含部分关键定义，更多的细节和扩展内容会在其他头文件中实现，
 以便代码结构清晰、易于维护。
 */

/*
 * Required ARM standard platform porting definitions
 接下来的内容将包含ARM标准平台移植所必需的定义。
 这些定义通常是ARM架构下各个平台都需要实现的基础配置，
 比如核心数量、电源域、最大电源管理级别等
 */


 /*
 \plat\arm\board\fvp\platform.mk：
# Default cluster count for FVP
FVP_CLUSTER_COUNT		:= 2
PLAT_ARM_CLUSTER_COUNT 在plat\arm\board\fvp\fconf\fconf_hw_config_getter.c 起到范围限制作用
它确保在 FVP 平台上，集群数量不会超过预定义的最大值，限制了uint32_t cpus_per_cluster数组的长度
在遍历设备树中的集群节点时遍历和断言的上限。如果设备树中集群数量超出这个上限，程序会触发断言，防止后续数组越界和数据错误。
 */
#define PLAT_ARM_CLUSTER_COUNT		U(FVP_CLUSTER_COUNT)

 /*
 FVP_TRUSTED_SRAM_SIZE:
# Size (in kilobytes) of the Trusted SRAM region to 
utilize when building for the FVP platform.
\plat\arm\board\fvp\platform.mk：
ifeq (${ENABLE_RME},1)
FVP_TRUSTED_SRAM_SIZE		:= 384
else
FVP_TRUSTED_SRAM_SIZE		:= 256
endif

\make_helpers\arch_features.mk
# Flag to enable Realm Management Extension (FEAT_RME).
ENABLE_RME				?=	0

PLAT_ARM_TRUSTED_SRAM_SIZE:
 * 定义 FVP 平台的受信任 SRAM 大小为 256 KB 或 384 KB，
 * 具体大小由FVP_TRUSTED_SRAM_SIZE控制，间接取决于 ENABLE_RME 的值。	

include\plat\arm\common\arm_def.h:
#ifdef PLAT_ARM_TRUSTED_SRAM_BASE
#define ARM_TRUSTED_SRAM_BASE		PLAT_ARM_TRUSTED_SRAM_BASE
#else
#define ARM_TRUSTED_SRAM_BASE		UL(0x04000000)
#endif 
 */
#define PLAT_ARM_TRUSTED_SRAM_SIZE	(FVP_TRUSTED_SRAM_SIZE * UL(1024))

/*
PLAT_ARM_TRUSTED_ROM_BASE 设置受信任ROM（只读存储器）的起始物理地址为 0x00000000。
PLAT_ARM_TRUSTED_ROM_SIZE 指定受信任ROM的大小为 0x04000000 字节，即64 MB。
0x04000000 = 4*16 * 16*16*(4*4)*16*16  = 64 * 1024 * 1024 = 64 MB
这两个宏定义通常用于配置受信任ROM的内存映射和访问权限
 */
#define PLAT_ARM_TRUSTED_ROM_BASE	UL(0x00000000)
#define PLAT_ARM_TRUSTED_ROM_SIZE	UL(0x04000000)	/* 64 MB */

/* 
PLAT_ARM_TRUSTED_DRAM_BASE 设置受信任 DRAM（动态随机存取存储器）的起始物理地址为 0x06000000。
PLAT_ARM_TRUSTED_DRAM_SIZE 指定受信任 DRAM 的大小为 0x02000000 字节，即 32 MB。
这两个宏定义通常用于配置受信任 DRAM 的内存映射和访问权限。

0x04000000 到 0x05FFFFFF 这段地址空间（共32MB）并没有被这两个宏定义覆盖, SRAM也只是顶多从0x04000000开始用几百KB
这说明ROM和DRAM之间是有间隔（空洞）的
 */
#define PLAT_ARM_TRUSTED_DRAM_BASE	UL(0x06000000)
#define PLAT_ARM_TRUSTED_DRAM_SIZE	UL(0x02000000)	/* 32 MB */

/*
\make_helpers\arch_features.mk
# Flag to enable Realm Management Extension (FEAT_RME).
ENABLE_RME				?=	0
 */
#if ENABLE_RME
#define PLAT_ARM_RMM_BASE		(RMM_BASE)
#define PLAT_ARM_RMM_SIZE		(RMM_LIMIT - RMM_BASE)

/* Protected physical address size */
#define PLAT_ARM_PPS			(SZ_1T)
#endif /* ENABLE_RME */

 /*
 SPD由make指令指定，如：
 	make \
    CROSS_COMPILE=aarch64-none-elf- \
    SPD=spmd \
	......
SPD（Secure Payload Dispatcher）参数用于选择安全世界的管理组件（如opteed、spmd、tspd等）。
如果你在make时没有指定 SPD=xxx，那么TF-A默认不会启用任何安全负载分发器（SPD）。
这意味着不会自动加载和启动BL32阶段（即不会加载安全世界固件，如OP-TEE、SPMC等）。
启动流程会从BL31（EL3固件）直接跳转到BL33（通常是U-Boot、UEFI或Linux内核等普通世界启动程序）。
 * 当启用 SPMD（Secure Partition Manager Dispatcher，安全分区管理分发器）功能时，
 * 下面的宏定义为 SPMC（Secure Partition Manager Core，安全分区管理核心）分配内存区域。
 * SPMC 的基地址设置为受信任 DRAM 的起始地址（PLAT_ARM_TRUSTED_DRAM_BASE），
 * 大小为 2MB（0x200000 字节）。
 * 该区域通常用于存放 BL32 阶段的镜像（如 SPMC 或 OP-TEE），
 */
#if defined(SPD_spmd)
#define PLAT_ARM_SPMC_BASE		PLAT_ARM_TRUSTED_DRAM_BASE
#define PLAT_ARM_SPMC_SIZE		UL(0x200000)  /* 2 MB */
#endif

/* Virtual address used by dynamic mem_protect for chunk_base */
/* 这个宏定义用于设置动态内存保护（mem_protect）功能中 chunk_base 的虚拟地址。
 * chunk_base 是一个内存保护机制，是动态分配的内存块的起始虚拟地址。
 * 所有受保护的内存块都以这个地址为基准进行管理和映射。用于确保安全分区的内存区域不会被非授权访问。
 * 这里设置的虚拟地址为 0xc0000000，这在32位虚拟地址空间中属于高端用户空间，
 * 通常不会与操作系统内核或固件的其他关键区域冲突，避免与普通世界（如Linux内核、U-Boot等）
 * 或安全世界（如OP-TEE、SPMC等）中常用的内存区域重叠
 * 用于隔离安全和非安全世界的内存访问。
 */
#define PLAT_ARM_MEM_PROTEC_VA_FRAME	UL(0xc0000000)

/* No SCP in FVP */
/*
 * FVP 平台没有 SCP（System Control Processor），因此相关的宏定义被设置为 0。
 * 这些宏定义通常用于配置 SCP 的内存区域和访问权限。
 * 在 FVP 平台上，这些宏定义不会被使用，因为没有 SCP 硬件。
 
 board\fvp\fdts\fvp_tb_fw_config.dts中有：
 #if ARM_IO_IN_DTB
arm-io_policies {
fip-handles {
compatible = "arm,io-fip-handle";
scp_bl2_uuid = "9766fd3d-89be-e849-ae5d-78a140608213";
......
scp_fw_key_uuid = "024221a1-f860-e411-8d9b-f33c0e15a014";
......
scp_fw_content_cert_uuid = "44be6f04-5e63-e411-b28b-73d8eaae9656";

这些字段即使在FVP上不用，也会保留模板，方便后续扩展或移植到有SCP的硬件平台。
 */
#define PLAT_ARM_SCP_TZC_DRAM1_SIZE	UL(0x0)


/*
 * 定义 FVP 平台的 DRAM2 区域的基地址和大小。
 * FVP 平台有 30GB 的 DRAM2 区域，地址范围为 0x880000000 到 0xF FFFF FFFF
 结束地址是是 0xF FFFF FFFF，参考FVP_DRAM3_END的计算：
 0x8 8000 0000 + 0x7 8000 0000 - 1U = 0x10 0000 0000 - 1U = 0xF FFFF FFFF
 * 这些地址通常用于存储操作系统和应用程序的运行时数据。
 */
#define PLAT_ARM_DRAM2_BASE	ULL(0x880000000) /* 36-bit range */
#define PLAT_ARM_DRAM2_SIZE	ULL(0x780000000) /* 30 GB */

#define FVP_DRAM3_BASE	ULL(0x8800000000) /* 40-bit range */
#define FVP_DRAM3_SIZE	ULL(0x7800000000) /* 480 GB */
#define FVP_DRAM3_END	(FVP_DRAM3_BASE + FVP_DRAM3_SIZE - 1U)

#define FVP_DRAM4_BASE	ULL(0x88000000000) /* 44-bit range */
#define FVP_DRAM4_SIZE	ULL(0x78000000000) /* 7.5 TB */
#define FVP_DRAM4_END	(FVP_DRAM4_BASE + FVP_DRAM4_SIZE - 1U)

#define FVP_DRAM5_BASE	ULL(0x880000000000) /* 48-bit range */
#define FVP_DRAM5_SIZE	ULL(0x780000000000) /* 120 TB */
#define FVP_DRAM5_END	(FVP_DRAM5_BASE + FVP_DRAM5_SIZE - 1U)

#define FVP_DRAM6_BASE	ULL(0x8800000000000) /* 52-bit range */
#define FVP_DRAM6_SIZE	ULL(0x7800000000000) /* 1920 TB */
#define FVP_DRAM6_END	(FVP_DRAM6_BASE + FVP_DRAM6_SIZE - 1U)

/*
 * On the FVP platform when using the EL3 SPMC implementation allocate the
 * datastore for tracking shared memory descriptors in the TZC DRAM section
 * to ensure sufficient storage can be allocated.
 * Provide an implementation of the accessor method to allow the datastore
 * details to be retrieved by the SPMC.
 * The SPMC will take care of initializing the memory region.
 */
 /*
 * 在 FVP 平台上，使用 EL3 SPMC （Secure Partition Manager Core）实现时，
 * 分配用于跟踪 TZC DRAM 部分中共享内存描述符的数据存储区
 * 以确保能够分配足够的存储空间。
 * 提供访问器方法的实现，以允许 SPMC 检索数据存储区
 * 的详细信息。
 * SPMC 将负责初始化内存区域。
 */
/*分配给共享内存描述符数据存储区的大小为512KB */
#define PLAT_SPMC_SHMEM_DATASTORE_SIZE 512 * 1024

/* Define memory configuration for device tree files. */
/*
 * PLAT_ARM_HW_CONFIG_SIZE 用于定义硬件配置的大小。
 * 指定设备树文件（device tree files）相关的硬件配置内存区域大小。
 * 这里的值为0x4000，即16 KB。
 * U()宏用于将常量转换为无符号整数类型，确保类型安全。
 * 这些宏定义通常用于配置设备树（Device Tree）文件中的硬件信息。
 */
#define PLAT_ARM_HW_CONFIG_SIZE			U(0x4000)


/*
以下#if 代码用于在启用SPMC_AT_EL3（即安全分区管理核心运行在EL3特权级）时，配置分区数量相关的参数。
这些宏主要用于限制和配置SPMC在EL3运行时可管理的分区数量
*/
#if SPMC_AT_EL3
/*
 * Number of Secure Partitions supported. 
 * SPMC at EL3, uses this count to configure the maximum number of supported
 * secure partitions.
 */
 /* 
 安全分区数量，SPMC在EL3运行时使用此计数来配置支持的最大安全分区数量。
 这里设置为1，表示只支持一个安全分区。
 */
#define SECURE_PARTITION_COUNT		1

/*
 * Number of Normal World Partitions supported.
 * SPMC at EL3, uses this count to configure the maximum number of supported
 * NWd partitions.
 */
 /* 
 非安全分区（普通世界分区）数量，SPMC在EL3运行时使用此计数来配置支持的最大非安全分区数量。
 这里设置为1，表示只支持一个非安全分区。
 */
#define NS_PARTITION_COUNT		1

/*
 * Number of Logical Partitions supported. 
 * SPMC at EL3, uses this count to configure the maximum number of supported
 * logical partitions.
 */
 /* 
 逻辑分区数量，SPMC在EL3运行时使用此计数来配置支持的最大逻辑分区数量。
 这里设置为1，表示只支持一个逻辑分区。
 */
#define MAX_EL3_LP_DESCS_COUNT		1

#endif /* SPMC_AT_EL3 */

/*
 * Load address of BL33 for this platform port	此平台端口的BL33加载地址
 */
 /*
 * PLAT_ARM_NS_IMAGE_BASE 定义了BL33非安全世界（NS）的映像基地址。
 * 这个地址通常用于加载非安全世界的固件或操作系统映像。
官方注释也说明该宏用于设置当前平台移植下BL33的加载地址。
宏的值为ARM_DRAM1_BASE + UL(0x8000000)，即在DRAM1的基地址上偏移0x8000000（128 MB）。
UL()宏用于将常量转换为无符号长整型，确保类型安全。
这样做的目的是将BL33镜像加载到内存的一个特定、安全且不会与其他固件阶段冲突的位置，
便于启动流程的管理和内存布局的规划。
 */
#define PLAT_ARM_NS_IMAGE_BASE		(ARM_DRAM1_BASE + UL(0x8000000))

#if TRANSFER_LIST //如果启用了TRANSFER_LIST（传递列表）宏
/*
 * PLAT_ARM_FW_HANDOFF_SIZE 用于启动流程中不同固件阶段（如BL31、BL33等）之间传递必要信息时，
 * 预留一块专用内存区域。
 */
#define PLAT_ARM_FW_HANDOFF_SIZE	U(0x5000) //固件交接（firmware handoff）信息区域的大小为 20 KB */
/*
 * PLAT_ARM_FW_HANDOFF_BASE 定义了固件交接信息区域的基地址。
 * 这个地址通常用于存储在不同固件阶段之间传递的信息，如BL31到BL33的传递列表。
 * 这里设置为非安全世界映像基地址减去固件交接信息区域的大小。
 * 把交接信息区域放在BL33镜像之前的那一段内存，即从FW_NS_HANDOFF_BASE开始，连续PLAT_ARM_FW_HANDOFF_SIZE字节，
 * 正好到PLAT_ARM_NS_IMAGE_BASE-1为止，避免交接区和BL33镜像重叠或覆盖BL33后面的空间，导致内存冲突
 */
#define FW_NS_HANDOFF_BASE		(PLAT_ARM_NS_IMAGE_BASE - PLAT_ARM_FW_HANDOFF_SIZE)

/*
 * PLAT_ARM_EL3_FW_HANDOFF_BASE 定义了EL3特权级固件用于交接信息的内存基地址。
 * 这个地址通常用于存储在EL3阶段传递给BL31的信息。
 * 这里设置为ARM_BL_RAM_BASE，即BL RAM（Boot Loader RAM）的基地址。

 include\plat\arm\common\arm_def.h：
 #define ARM_BL_RAM_BASE			(ARM_SHARED_RAM_BASE + ARM_SHARED_RAM_SIZE)

 */
#define PLAT_ARM_EL3_FW_HANDOFF_BASE	ARM_BL_RAM_BASE

/*
 * PLAT_ARM_EL3_FW_HANDOFF_LIMIT 定义了EL3特权级固件交接信息的内存限制地址。
 * 这个地址通常用于标识EL3阶段传递给BL31的信息区域的结束位置。
 * 这里设置为EL3交接基地址加上固件交接信息区域的大小。
 * 这样做是为了确保在EL3阶段传递的信息不会超过预定义的大小范围。
 */
#define PLAT_ARM_EL3_FW_HANDOFF_LIMIT	PLAT_ARM_EL3_FW_HANDOFF_BASE + PLAT_ARM_FW_HANDOFF_SIZE

/*
#if RESET_TO_BL31 表示当宏RESET_TO_BL31被定义且其值为非零时后续的代码块才会被编译进最终的程序

make_helpers\defaults.mk：
# By default, BL1 acts as the reset handler, not BL31
RESET_TO_BL31			:= 0

 * PLAT_ARM_TRANSFER_LIST_DTB_OFFSET 定义了传递列表（transfer list）设备树（DTB）的偏移量。
 * 这个偏移量通常用于在固件交接信息区域中存储传递列表的设备树。
 * 计算方式是：以非安全世界固件交接信息区域的基地址（FW_NS_HANDOFF_BASE）为起点，
 * 加上传递列表的具体偏移量（TRANSFER_LIST_DTB_OFFSET）

 FW_NS_HANDOFF_BASE在本文件前文有定义，而TRANSFER_LIST_DTB_OFFSET在plat\arm\board\fvp\platform.mk
ifeq (${TRANSFER_LIST}, 1)
ifeq ($(RESET_TO_BL31), 1)
FW_HANDOFF_SIZE			:=	20000
TRANSFER_LIST_DTB_OFFSET	:=	0x20
$(eval $(call add_define,TRANSFER_LIST_DTB_OFFSET))
endif
endif

 */
#if RESET_TO_BL31
#define PLAT_ARM_TRANSFER_LIST_DTB_OFFSET	FW_NS_HANDOFF_BASE + TRANSFER_LIST_DTB_OFFSET
#endif

#else /* 如果没有启用TRANSFER_LIST宏 */
/*当没有启用TRANSFER_LIST宏时，固件交接区不需要分配内存*/
#define PLAT_ARM_FW_HANDOFF_SIZE	U(0)
#endif /* TRANSFER_LIST */

/*
 * PLAT_ARM_MMAP_ENTRIES depends on the number of entries in the
 * plat_arm_mmap array defined for each BL stage.
 PLAT_ARM_MMAP_ENTRIES【内存映射表（mmap）】的条目数量取决于
 每个 BL 阶段定义的 plat_arm_mmap 数组中的条目数。
 */
#if defined(IMAGE_BL31) // 如果BL31镜像被定义
# if SPM_MM /* 如果启用了SPM内存管理 */
#  define PLAT_ARM_MMAP_ENTRIES		10	// BL31的内存映射条目数为10
#  define MAX_XLAT_TABLES		9	// 最大转换表数量为9
#  define PLAT_SP_IMAGE_MMAP_REGIONS	30	// 安全分区映像内存映射区域数为30
#  define PLAT_SP_IMAGE_MAX_XLAT_TABLES	10	// 安全分区映像最大转换表数量为10
# elif SPMC_AT_EL3	 // 如果没有启用SPM内存管理，且SPMC在EL3运行
#  define PLAT_ARM_MMAP_ENTRIES		13	// BL31的内存映射条目数为13
#  define MAX_XLAT_TABLES		11	// 最大转换表数量为11
#  define PLAT_SP_IMAGE_MMAP_REGIONS	30	// 安全分区映像内存映射区域数为30
#  define PLAT_SP_IMAGE_MAX_XLAT_TABLES	10	// 安全分区映像最大转换表数量为10
# else
#  define PLAT_ARM_MMAP_ENTRIES		9	// BL31的内存映射条目数为9
/*
USE_DEBUGFS设置为 1 时，此选项会通过 BL31 公开虚拟文件系统接口，作为 SiP SMC 功能。
默认为禁用 (0)。
make_helpers\defaults.mk:
# Build option to add debugfs support
USE_DEBUGFS			:= 0
*/
#  if USE_DEBUGFS	// 如果启用了调试文件系统
#   if ENABLE_RME	// 如果启用了RME（Realm Management Extension），但ENABLE_RME默认为0
#    define MAX_XLAT_TABLES		9 // 最大转换表数量为9
#   else	// !USE_DEBUGFS，如果没有启用RME
#    define MAX_XLAT_TABLES		8	// 最大转换表数量为8
#   endif	// 结束RME相关的条件编译
#  else	// !USE_DEBUGFS，如果没有启用调试文件系统
#   if ENABLE_RME	// 如果启用了RME（Realm Management Extension）
#    define MAX_XLAT_TABLES		8	// 最大转换表数量为8
#   elif DRTM_SUPPORT	// 如果启用了DRTM（Dynamic Root of Trust for Measurement），DRTM默认为0
#    define MAX_XLAT_TABLES		8	// 最大转换表数量为8
#   else
#    define MAX_XLAT_TABLES		7	// 最大转换表数量为7
#   endif /* ENABLE_RME */
#  endif	 /* USE_DEBUGFS */
# endif /* SPM_MM || SPMC_AT_EL3 */
#elif defined(IMAGE_BL32)	// 没定义IMAGE_BL31，如果BL32镜像被定义
# if SPMC_AT_EL3	 // 如果SPMC在EL3运行
#  define PLAT_ARM_MMAP_ENTRIES		270	// BL32的内存映射条目数为270
#  define MAX_XLAT_TABLES		10	// 最大转换表数量为10
# else	// 如果没有启用SPMC在EL3运行
#  define PLAT_ARM_MMAP_ENTRIES		9	// BL32的内存映射条目数为9
#  define MAX_XLAT_TABLES		6	// 最大转换表数量为6
# endif	 /* SPMC_AT_EL3 */
#elif !USE_ROMLIB	// 如果没有定义BL31、BL32镜像，且没有启用ROMLIB（只读内存库，USE_ROMLIB本身就默认为0）
# if ENABLE_RME && defined(IMAGE_BL2)	// 如果启用了RME且BL2镜像被定义
#  define PLAT_ARM_MMAP_ENTRIES		12	// BL2的内存映射条目数为12
#  define MAX_XLAT_TABLES		6	// 最大转换表数量为6
# else	// 如果没有启用RME或BL2镜像没有被定义
#  define PLAT_ARM_MMAP_ENTRIES		12	// BL2的内存映射条目数为12
#  define MAX_XLAT_TABLES		5	// 最大转换表数量为5
# endif /* (IMAGE_BL2 && ENABLE_RME) */
#else	 // 如果没有定义BL31、BL32镜像，且启用了ROMLIB（只读内存库）
# define PLAT_ARM_MMAP_ENTRIES		12	// BL2的内存映射条目数为12
/*
1.至少有一个安全负载分发器（SPD）被启用，即SPD_tspd、SPD_opteed或SPD_spmd中至少有一个被定义。
2.当前正在编译BL2阶段（IMAGE_BL2被定义）。
3.启用了度量启动（MEASURED_BOOT被定义）。
*/
# if (defined(SPD_tspd) || defined(SPD_opteed) || defined(SPD_spmd)) && \
defined(IMAGE_BL2) && MEASURED_BOOT	
#  define MAX_XLAT_TABLES		7	// 最大转换表数量为7
# else	// 如果没有满足上述条件
#  define MAX_XLAT_TABLES		6	// 最大转换表数量为6
# endif /* (SPD_tspd || SPD_opteed || SPD_spmd) && IMAGE_BL2 && MEASURED_BOOT */
#endif	  /* if defined(IMAGE_BL31) */

/*
 * PLAT_ARM_MAX_BL1_RW_SIZE is calculated using the current BL1 RW debug size
 * plus a little space for growth.
 * In case of PSA Crypto API, few algorithms like ECDSA needs bigger BL1 RW
 * area.
* PLAT_ARM_MAX_BL1_RW_SIZE 是根据当前 BL1 RW 调试大小计算得出的，并预留了一点增长空间。
* 对于 PSA Crypto API，少数算法（例如 ECDSA）需要更大的 BL1 RW区域。
所以条件编译部分根据不同配置动态调整BL1 RW区域的大小：
1.如果启用了TF_MBEDTLS_KEY_ALG_ID == TF_MBEDTLS_RSA_AND_ECDSA（即同时支持RSA和ECDSA算法）、
PSA_CRYPTO（启用PSA加密API）或FVP_TRUSTED_SRAM_SIZE == 512（受信任SRAM大小为512KB），
则将最大RW区域设置为0xD000（52KB）。
2.否则，默认设置为0xB000（44KB）。
 */
#if TF_MBEDTLS_KEY_ALG_ID == TF_MBEDTLS_RSA_AND_ECDSA || PSA_CRYPTO || \
FVP_TRUSTED_SRAM_SIZE == 512
#define PLAT_ARM_MAX_BL1_RW_SIZE	UL(0xD000)
#else
#define PLAT_ARM_MAX_BL1_RW_SIZE	UL(0xB000)
#endif

/*
 * PLAT_ARM_MAX_ROMLIB_RW_SIZE is define to use a full page
 * PLAT_ARM_MAX_ROMLIB_RW_SIZE 定义为使用整页（通常为4KB，0x1000）
 */
#if USE_ROMLIB	// 如果启用了ROMLIB（只读内存库）
#define PLAT_ARM_MAX_ROMLIB_RW_SIZE	UL(0x1000)	//最大只读内存库（ROMLIB）可读写区域大小为4KB
#define PLAT_ARM_MAX_ROMLIB_RO_SIZE	UL(0xe000)	//最大只读内存库（ROMLIB）只读区域大小为56KB
#define FVP_BL2_ROMLIB_OPTIMIZATION	UL(0x5000)	// FVP BL2 ROMLIB优化大小为20KB
#else	// 如果没有启用ROMLIB（只读内存库），就不分配相关内存
#define PLAT_ARM_MAX_ROMLIB_RW_SIZE	UL(0)
#define PLAT_ARM_MAX_ROMLIB_RO_SIZE	UL(0)
#define FVP_BL2_ROMLIB_OPTIMIZATION UL(0)
#endif

/*
 * Set the maximum size of BL2 to be close to half of the Trusted SRAM.
 * Maximum size of BL2 increases as Trusted SRAM size increases.
 * 将 BL2 的最大大小设置为接近 Trusted SRAM 的一半。
 * 随着 Trusted SRAM 大小的增加，BL2 的最大大小也会增加。
 */
/* PLAT_ARM_MAX_BL2_SIZE 定义了 BL2 阶段的最大大小。
 * 这个大小主要取决于是否启用了加密支持（CRYPTO_SUPPORT）和是否将 BL31 放在 DRAM 中（ARM_BL31_IN_DRAM）。
 * 如果启用了加密支持，BL2 的最大大小会根据受信任 SRAM 的大小和其他因素进行计算。
 * 如果没有启用加密支持且 BL31 在 DRAM 中，则 BL2 可以使用几乎所有的受信任 SRAM。
 * 否则，默认情况下 BL2 的最大大小为受信任 SRAM 大小的一半，减去一些预留空间。
 */
#if CRYPTO_SUPPORT	// 如果启用了加密支持，CRYPTO_SUPPORT的范围是[0,3]，其值在Makefile中被计算，但是可以被override

/*
只有当TF_MBEDTLS_KEY_ALG_ID等于TF_MBEDTLS_RSA_AND_ECDSA（即同时支持RSA和ECDSA算法），
或者COT_DESC_IN_DTB宏被定义且为非零时，后续的代码块才会被编译进最终程序。
COT_DESC_IN_DTB等参数可以控制COT（Chain of Trust，信任链）描述符是静态写在代码里，
还是以设备树（DTB）节点和属性的形式存储在配置文件中。COT_DESC_IN_DTB默认为0（来自defaults.mk）
*/
#if (TF_MBEDTLS_KEY_ALG_ID == TF_MBEDTLS_RSA_AND_ECDSA) || COT_DESC_IN_DTB

/* 
include\lib\xlat_tables\xlat_tables_defs.h:
#define FOUR_KB_SHIFT		U(12)
#define PAGE_SIZE_SHIFT		FOUR_KB_SHIFT
#define PAGE_SIZE		(UL(1) << PAGE_SIZE_SHIFT)
所以PAGE_SIZE的值为 2^12 = 4096（0x1000），即4KB。
将受信任SRAM（PLAT_ARM_TRUSTED_SRAM_SIZE）的一半，减去两页（2 * PAGE_SIZE）的空间，
再减去ROMLIB优化所需的空间（FVP_BL2_ROMLIB_OPTIMIZATION）。
通过减去两页和ROMLIB优化空间，进一步确保内存不会发生重叠或冲突
 */
# define PLAT_ARM_MAX_BL2_SIZE	((PLAT_ARM_TRUSTED_SRAM_SIZE / 2) - \
				 (2 * PAGE_SIZE) - \
				 FVP_BL2_ROMLIB_OPTIMIZATION)
#else	/* 如果做不到(TF_MBEDTLS_KEY_ALG_ID == TF_MBEDTLS_RSA_AND_ECDSA) || COT_DESC_IN_DTB */

/* 
将受信任SRAM（PLAT_ARM_TRUSTED_SRAM_SIZE）的一半，减去三页（3 * PAGE_SIZE）的空间，
再减去ROMLIB优化所需的空间（FVP_BL2_ROMLIB_OPTIMIZATION）。
这样做是为了确保BL2的大小不会超过受信任SRAM的一半，并且留出足够的空间用于其他用途。
通过减去三页空间，可以为堆栈、页表或其他特殊用途预留内存，避免内存重叠或冲突。
再减去ROMLIB优化空间，确保ROMLIB相关功能有独立的内存区域。
 */
# define PLAT_ARM_MAX_BL2_SIZE	((PLAT_ARM_TRUSTED_SRAM_SIZE / 2) - \
				 (3 * PAGE_SIZE) - \
				 FVP_BL2_ROMLIB_OPTIMIZATION)
#endif	 /* (TF_MBEDTLS_KEY_ALG_ID == TF_MBEDTLS_RSA_AND_ECDSA) || COT_DESC_IN_DTB */

/*
在64位下，ARM_BL31_IN_DRAM被设置为0：
plat\arm\common\arm_common.mk：
  ARM_BL31_IN_DRAM		:=	0
*/
#elif ARM_BL31_IN_DRAM	// 如果没有启用CRYPTO_SUPPORT（==0）且， BL31 放在 DRAM 中，
/* When ARM_BL31_IN_DRAM is set, BL2 can use almost all of Trusted SRAM. */
/*用于指定BL2阶段（Boot Loader 2）可用的最大内存空间。其值为0x1F000（124 KB）减去FVP_BL2_ROMLIB_OPTIMIZATION（ROMLIB优化所需的空间）*/
# define PLAT_ARM_MAX_BL2_SIZE	(UL(0x1F000) - FVP_BL2_ROMLIB_OPTIMIZATION)
#else	// 如果没有启用CRYPTO_SUPPORT（==0）且， BL31 不放在 DRAM 中（ARM_BL31_IN_DRAM == 0）
/**
 * Default to just under half of SRAM to ensure there's enough room for really
 * large BL31 build configurations when using the default SRAM size (256 Kb).
 默认设置为略低于 SRAM 的一半，以确保在使用默认 SRAM 大小 (256 KB) 时，
 有足够的空间用于真正大型的 BL31 构建配置。
 */
 /*
计算方式如下：
1.先将受信任SRAM总大小（PLAT_ARM_TRUSTED_SRAM_SIZE）除以3，得到三分之一的SRAM空间。
2.用按位与操作& ~PAGE_SIZE_MASK将结果对齐到页边界（通常是向下对齐到最近的页大小倍数），以保证内存分配的对齐性和安全性。
3.再减去一页的大小（PAGE_SIZE），为系统堆栈、页表或其他特殊用途预留空间。
4.最后减去FVP_BL2_ROMLIB_OPTIMIZATION，为ROMLIB优化功能预留专用内存区域。
这种写法确保BL2不会占用过多的受信任SRAM，同时为系统的其他关键功能预留了必要的内存空间，

include\lib\xlat_tables\xlat_tables_defs.h:
#define PAGE_SIZE_MASK		(PAGE_SIZE - UL(1))
本文件没有直接引入这个文件，但是通过：
include <plat/arm/common/arm_def.h> / include <plat/common/common_def.h>
来得到了PAGE_SIZE_MASK
"~PAGE_SIZE_MASK"的是将"0x0……0FFF"中的二进制数取反，
从而得到一个二进制数，该数的低12位为0，其他位为1。

举例子：假设有一个地址 X = 0x12345，其二进制为：
0001 0010 0011 0100 0101， 它和 0xF……F000 做按位与后：
低12位全部变成0，高位保持不变
结果就是 0x12000，也就是小于等于原始值的最大的一个4KB整数倍的数。
 */
#define PLAT_ARM_MAX_BL2_SIZE                                               \
	(((PLAT_ARM_TRUSTED_SRAM_SIZE / 3) & ~PAGE_SIZE_MASK) - PAGE_SIZE - \
	 FVP_BL2_ROMLIB_OPTIMIZATION)
#endif	/* CRYPTO_SUPPORT */

/* 在启用RESET_TO_BL31宏时，定义BL31阶段（Boot Loader 31）可用的最大内存空间*/
#if RESET_TO_BL31
/* Size of Trusted SRAM - the first 4KB of shared memory - GPT L0 Tables */
/* 可信 SRAM 的大小 - 共享内存的前 4KB - GPT L0 表 */
/*
宏PLAT_ARM_MAX_BL31_SIZE的计算方式是：
用PLAT_ARM_TRUSTED_SRAM_SIZE（受信任SRAM总大小）
减去ARM_SHARED_RAM_SIZE（共享内存大小，通常为4KB）和ARM_L0_GPT_SIZE（GPT L0表的大小）。
这样可以确保BL31不会占用共享内存和GPT表所需的空间，避免内存冲突

include\plat\arm\common\arm_def.h：（ARM_SHARED_RAM_SIZE == 4KB）
#define ARM_SHARED_RAM_SIZE		UL(0x00001000)

#if ENABLE_RME
…………
#define ARM_L0_GPT_SIZE			UL(0x00002000)	
#else
#define ARM_L0_GPT_SIZE			UL(0)
#endif
*/
#define PLAT_ARM_MAX_BL31_SIZE		(PLAT_ARM_TRUSTED_SRAM_SIZE - \
					 ARM_SHARED_RAM_SIZE - \
					 ARM_L0_GPT_SIZE)
#else /* 如果没有定义RESET_TO_BL31宏 */


/*
 * Since BL31 NOBITS overlays BL2 and BL1-RW, PLAT_ARM_MAX_BL31_SIZE is
 * calculated using the current BL31 PROGBITS debug size plus the sizes of
 * BL2 and BL1-RW.
 * Size of the BL31 PROGBITS increases as the SRAM size increases.
 由于BL31的NOBITS段会覆盖BL2和BL1-RW，因此最大空间的计算（PLAT_ARM_MAX_BL31_SIZE）
 要考虑BL31的PROGBITS调试大小以及BL2和BL1-RW的大小。
 随着SRAM容量的增加，BL31的PROGBITS段也会变大

 NOBITS段：
 在ELF（可执行与可链接格式）文件结构中，段（section）有不同的类型。
 NOBITS段（如.bss）是一种特殊的段类型，表示“只分配但不占用文件空间的数据”

 PROGBITS段：
 PROGBITS段（如.text、.data）是ELF文件中实际包含代码和已初始化数据的段。
 通常可通过BL31镜像中所有PROGBITS类型段（即实际需要加载到内存的代码和数据段）的总大小估算BL31在内存中实际占用的空间
 
 PLAT_ARM_TRUSTED_SRAM_SIZE、PLAT_ARM_FW_HANDOFF_SIZE、ARM_L0_GPT_SIZE参考前面的注释

 include\plat\arm\common\arm_def.h：（ARM_SHARED_RAM_SIZE == 4KB）
 #define ARM_SHARED_RAM_SIZE		UL(0x00001000)

 include\plat\arm\common\arm_def.h：（ARM_FW_CONFIGS_SIZE == 8KB）
 #define ARM_FW_CONFIGS_SIZE		(PAGE_SIZE * 2)
 */
#if TRANSFER_LIST //启用TRANSFER_LIST时，需要为固件阶段间的动态数据交换（如BL31到BL33的启动参数）预留专用缓冲区
#define PLAT_ARM_MAX_BL31_SIZE                              \
	(PLAT_ARM_TRUSTED_SRAM_SIZE - ARM_SHARED_RAM_SIZE - \
	 PLAT_ARM_FW_HANDOFF_SIZE - ARM_L0_GPT_SIZE)
#else //未启用TRANSFER_LIST时，使用标准配置区域存储固件参数（如设备树信息）
#define PLAT_ARM_MAX_BL31_SIZE		(PLAT_ARM_TRUSTED_SRAM_SIZE - \
					 ARM_SHARED_RAM_SIZE - \
					 ARM_FW_CONFIGS_SIZE - ARM_L0_GPT_SIZE)
#endif /* TRANSFER_LIST */
#endif /* RESET_TO_BL31 */

#ifndef __aarch64__ //仅在32位ARM架构下生效，因SP_MIN通常运行在AArch32模式
#if RESET_TO_SP_MIN
/* Size of Trusted SRAM - the first 4KB of shared memory */
#define PLAT_ARM_MAX_BL32_SIZE		(PLAT_ARM_TRUSTED_SRAM_SIZE - \
					 ARM_SHARED_RAM_SIZE)
#else
/*
 * Since BL32 NOBITS overlays BL2 and BL1-RW, PLAT_ARM_MAX_BL32_SIZE is
 * calculated using the current SP_MIN PROGBITS debug size plus the sizes of
 * BL2 and BL1-RW
 */
#if TRANSFER_LIST
# define PLAT_ARM_MAX_BL32_SIZE		(PLAT_ARM_TRUSTED_SRAM_SIZE - \
					 ARM_SHARED_RAM_SIZE - \
					 PLAT_ARM_FW_HANDOFF_SIZE)
#else
# define PLAT_ARM_MAX_BL32_SIZE		(PLAT_ARM_TRUSTED_SRAM_SIZE - \
					 ARM_SHARED_RAM_SIZE - \
					 ARM_FW_CONFIGS_SIZE)
#endif /* TRANSFER_LIST */
#endif /* RESET_TO_SP_MIN */
#endif /* ifndef __aarch64__ */

/*
 * Size of cacheable stacks
 * 可缓存堆栈的大小
 在ATF，每个固件阶段（BL1/BL2/BL31等）都是独立编译的。
 当使用make all时，构建系统会依次执行以下关键步骤：
1.分阶段编译
 # 实际编译流程（简化版）
gcc -DIMAGE_BL1  -o bl1.o  # 编译BL1时只定义IMAGE_BL1
gcc -DIMAGE_BL2  -o bl2.o  # 编译BL2时只定义IMAGE_BL2
gcc -DIMAGE_BL31 -o bl31.o # 编译BL31时只定义IMAGE_BL31
2.内存隔离机制
每个阶段的.bss/.data段都有独立地址空间，通过链接脚本严格隔离：
3.最终内存布局通过链接器脚本拼接

CRYPTO_SUPPORT 取值范围为[0,3]，其值在Makefile中被计算，与MEASURED_BOOT、
TRUSTED_BOARD_BOOT以及DRTM_SUPPORT有关

DRTM_SUPPORT
 * DRTM_SUPPORT 取值范围为[0,1]，其值在defaults.mk中被定义，默认为0。
 * 当DRTM_SUPPORT为1时，表示启用动态根信任测量（Dynamic Root of Trust for Measurement）。
 * 否则为0，表示不启用。

SPMC_AT_EL3
* DRTM_SUPPORT 取值范围为[0,1]，其值在defaults.mk中被定义，默认为0。
 * 当SPMC_AT_EL3为1时，表示安全分区管理核心（SPMC）运行在EL3特权级。
 * 否则为0，表示SPMC运行在EL2或其他特权级
 */
#if defined(IMAGE_BL1)
# if CRYPTO_SUPPORT
#  define PLATFORM_STACK_SIZE		UL(0x1000)
# else
#  define PLATFORM_STACK_SIZE		UL(0x500)
# endif /* CRYPTO_SUPPORT */
#elif defined(IMAGE_BL2)
# if CRYPTO_SUPPORT
#  define PLATFORM_STACK_SIZE		UL(0x1000)
# else
#  define PLATFORM_STACK_SIZE		UL(0x600)
# endif /* CRYPTO_SUPPORT */
#elif defined(IMAGE_BL2U)
# define PLATFORM_STACK_SIZE		UL(0x400)
#elif defined(IMAGE_BL31)
# if DRTM_SUPPORT
#  define PLATFORM_STACK_SIZE		UL(0x1000)
# else
#  define PLATFORM_STACK_SIZE		UL(0x800)
# endif /* DRTM_SUPPORT */
#elif defined(IMAGE_BL32)
# if SPMC_AT_EL3
#  define PLATFORM_STACK_SIZE		UL(0x1000)
# else
#  define PLATFORM_STACK_SIZE		UL(0x440)
# endif /* SPMC_AT_EL3 */
#elif defined(IMAGE_RMM)
# define PLATFORM_STACK_SIZE		UL(0x440)
#endif

 /*
 如果平台移植使用 IO 存储框架，则还必须定义PLAT_ARM_MAX_IO_DEVICES、PLAT_ARM_MAX_IO_HANDLES、MAX_IO_BLOCK_DEVICES：
 PLAT_ARM_MAX_IO_DEVICES 定义已注册 IO 设备的最大数量，即实际控制IO存储框架中所有注册设备的总数
 ，包括：物理存储设备（eMMC/NOR Flash）、虚拟设备（FIP/内存盘）、文件系统驱动（FAT/EXT4） 、 块设备抽象层
 这里设置为3，表示最多支持3个IO设备

 PLAT_ARM_MAX_IO_HANDLES 定义打开的 IO 句柄的最大数量，即控制同时操作的IO实体数量
 典型场景：并行加载多个固件镜像（BL2/BL31/BL32）、同时访问FIP包内的多个文件（tb_fw.bin + soc_fw.bin）、嵌套文件操作（通过不同句柄读写不同分区）
 这里设置为4，表示最多支持4个并发IO操作

配置依据分析：
1. 存储介质类型：
   - 每个物理设备（eMMC/SD/NOR）需1个设备槽位
   - 每个虚拟设备（FIP/RAMDISK）需1个设备槽位
   - 文件系统驱动（FAT/EXT4）需独立注册为逻辑设备

2. 启动阶段需求：
   BL2阶段典型句柄消耗：
   - FIP容器：1 handle
   - BL31镜像：1 handle  
   - BL32镜像：1 handle
   - 证书链：1 handle
 * 默认配置经过ARM官方验证，适用于以下标准场景：
 * 1. 基础存储拓扑：1个物理设备(eMMC) + 1个虚拟设备(FIP) + 1个文件系统驱动
 * 2. 典型启动流程：BL2顺序加载BL31/BL32/BL33
 * 3. FIP包内单文件访问
 */
#define MAX_IO_DEVICES			3
#define MAX_IO_HANDLES			4

/* Reserve the last block of flash for PSCI MEM PROTECT flag */
/* 为 PSCI MEM PROTECT 标志保留闪存的最后一个块 */
/*
本文件使用了：#include <plat/arm/board/common/v2m_def.h>，在这个文件里有：
// NOR Flash 
#define V2M_FLASH0_BASE			(V2M_OFFSET + UL(0x08000000))
#define V2M_FLASH0_SIZE			UL(0x04000000)
#define V2M_FLASH1_BASE			(V2M_OFFSET + UL(0x0c000000))
#define V2M_FLASH1_SIZE			UL(0x04000000)
#define V2M_FLASH_BLOCK_SIZE		UL(0x00040000)  //256 KB 

V2M是Versatile Express Memory Map的缩写，特指ARM开发板的标准内存映射规范。
这是ARM公司为其Versatile Express系列开发板定义的硬件地址布局标准。
需要三处修改的原因：

平台定义文件（platform_def.h）：
控制BL1/BL2的编译时地址计算和内存映射初始化

链接脚本（.ld.S）：
决定镜像加载的绝对地址（需保持与平台定义一致）

设备树（.dts）：
为BL33阶段（如U-Boot/Linux）提供运行时硬件信息

*/
#define PLAT_ARM_FLASH_IMAGE_BASE	V2M_FLASH0_BASE
#define PLAT_ARM_FLASH_IMAGE_MAX_SIZE	(V2M_FLASH0_SIZE - V2M_FLASH_BLOCK_SIZE)

#if ARM_GPT_SUPPORT
/*
 * Offset of the FIP in the GPT image. BL1 component uses this option
 * as it does not load the partition table to get the FIP base
 * address. At sector 34 by default (i.e. after reserved sectors 0-33)
 * Offset = 34 * 512(sector size) = 17408 i.e. 0x4400
 * GPT 映像中 FIP 的偏移量。BL1 组件使用此选项因为它不会加载分区表来获取 FIP 基址。
 * 默认位于扇区 34（即保留扇区 0-33 之后），偏移量 = 34 * 512（扇区大小）= 17408，即 0x4400
 
 */
#define PLAT_ARM_FIP_OFFSET_IN_GPT	0x4400
#endif /* ARM_GPT_SUPPORT */

#define PLAT_ARM_NVM_BASE		V2M_FLASH0_BASE
#define PLAT_ARM_NVM_SIZE		(V2M_FLASH0_SIZE - V2M_FLASH_BLOCK_SIZE)

/*
 * PL011 related constants
 */
#define PLAT_ARM_BOOT_UART_BASE		V2M_IOFPGA_UART0_BASE
#define PLAT_ARM_BOOT_UART_CLK_IN_HZ	V2M_IOFPGA_UART0_CLK_IN_HZ

#define PLAT_ARM_RUN_UART_BASE		V2M_IOFPGA_UART1_BASE
#define PLAT_ARM_RUN_UART_CLK_IN_HZ	V2M_IOFPGA_UART1_CLK_IN_HZ

#define PLAT_ARM_CRASH_UART_BASE	PLAT_ARM_RUN_UART_BASE
#define PLAT_ARM_CRASH_UART_CLK_IN_HZ	PLAuartT_ARM_RUN_UART_CLK_IN_HZ

#define PLAT_ARM_TSP_UART_BASE		V2M_IOFPGA_UART2_BASE
#define PLAT_ARM_TSP_UART_CLK_IN_HZ	V2M_IOFPGA_UART2_CLK_IN_HZ

#define PLAT_ARM_TRP_UART_BASE		V2M_IOFPGA_UART3_BASE
#define PLAT_ARM_TRP_UART_CLK_IN_HZ	V2M_IOFPGA_UART3_CLK_IN_HZ

#define PLAT_FVP_SMMUV3_BASE		UL(0x2b400000)
#define PLAT_ARM_SMMUV3_ROOT_REG_OFFSET UL(0x20000)

/* CCI related constants */
#define PLAT_FVP_CCI400_BASE		UL(0x2c090000)
#define PLAT_FVP_CCI400_CLUS0_SL_PORT	3
#define PLAT_FVP_CCI400_CLUS1_SL_PORT	4

/* CCI-500/CCI-550 on Base platform */
#define PLAT_FVP_CCI5XX_BASE		UL(0x2a000000)
#define PLAT_FVP_CCI5XX_CLUS0_SL_PORT	5
#define PLAT_FVP_CCI5XX_CLUS1_SL_PORT	6

/* CCN related constants. Only CCN 502 is currently supported */
#define PLAT_ARM_CCN_BASE		UL(0x2e000000)
#define PLAT_ARM_CLUSTER_TO_CCN_ID_MAP	1, 5, 7, 11

/* System timer related constants */
#define PLAT_ARM_NSTIMER_FRAME_ID	U(1)

/* Mailbox base address */
#define PLAT_ARM_TRUSTED_MAILBOX_BASE	ARM_TRUSTED_SRAM_BASE

/* PCIe memory region 1 (Base Platform RevC only) */
#define PLAT_ARM_PCI_MEM_1_BASE		(ULL(0x50000000))
#define PLAT_ARM_PCI_MEM_1_SIZE		(SZ_256M) /* 256MB */

/*
 * PCIe memory region 2 (Base Platform RevC only)
 * The full size of the second PCI memory region is 256GB
 * but for now we only allocate the L1 GPTs for the first 3GB.
 */
#define PLAT_ARM_PCI_MEM_2_BASE		(ULL(0x4000000000))
#define	PLAT_ARM_PCI_MEM_2_SIZE		(3 * SZ_1G) /* 3GB */

/* TrustZone controller related constants
 *
 * Currently only filters 0 and 2 are connected on Base FVP.
 * Filter 0 : CPU clusters (no access to DRAM by default)
 * Filter 1 : not connected
 * Filter 2 : LCDs (access to VRAM allowed by default)
 * Filter 3 : not connected
 * Programming unconnected filters will have no effect at the
 * moment. These filter could, however, be connected in future.
 * So care should be taken not to configure the unused filters.
 *
 * Allow only non-secure access to all DRAM to supported devices.
 * Give access to the CPUs and Virtio. Some devices
 * would normally use the default ID so allow that too.
 */
#define PLAT_ARM_TZC_BASE		UL(0x2a4a0000)
#define PLAT_ARM_TZC_FILTERS		TZC_400_REGION_ATTR_FILTER_BIT(0)

#define PLAT_ARM_TZC_NS_DEV_ACCESS	(				\
		TZC_REGION_ACCESS_RDWR(FVP_NSAID_DEFAULT)	|	\
		TZC_REGION_ACCESS_RDWR(FVP_NSAID_PCI)		|	\
		TZC_REGION_ACCESS_RDWR(FVP_NSAID_AP)		|	\
		TZC_REGION_ACCESS_RDWR(FVP_NSAID_VIRTIO)	|	\
		TZC_REGION_ACCESS_RDWR(FVP_NSAID_VIRTIO_OLD))

/*
 * GIC related constants to cater for both GICv2 and GICv3 instances of an
 * FVP. They could be overridden at runtime in case the FVP implements the
 * legacy VE memory map.
 */
#define PLAT_ARM_GICD_BASE		BASE_GICD_BASE
#define PLAT_ARM_GICR_BASE		BASE_GICR_BASE
#define PLAT_ARM_GICC_BASE		BASE_GICC_BASE

/*
 * Define a list of Group 1 Secure and Group 0 interrupts as per GICv3
 * terminology. On a GICv2 system or mode, the lists will be merged and treated
 * as Group 0 interrupts.
 */
#define PLAT_ARM_G1S_IRQ_PROPS(grp) \
	ARM_G1S_IRQ_PROPS(grp), \
	INTR_PROP_DESC(FVP_IRQ_TZ_WDOG, GIC_HIGHEST_SEC_PRIORITY, (grp), \
			GIC_INTR_CFG_LEVEL), \
	INTR_PROP_DESC(FVP_IRQ_SEC_SYS_TIMER, GIC_HIGHEST_SEC_PRIORITY, (grp), \
			GIC_INTR_CFG_LEVEL)

#define PLAT_ARM_G0_IRQ_PROPS(grp)	ARM_G0_IRQ_PROPS(grp)

#if SDEI_IN_FCONF
#define PLAT_SDEI_DP_EVENT_MAX_CNT	ARM_SDEI_DP_EVENT_MAX_CNT
#define PLAT_SDEI_DS_EVENT_MAX_CNT	ARM_SDEI_DS_EVENT_MAX_CNT
#else
  #if PLATFORM_TEST_RAS_FFH || PLATFORM_TEST_FFH_LSP_RAS_SP
  #define PLAT_ARM_PRIVATE_SDEI_EVENTS \
	ARM_SDEI_PRIVATE_EVENTS, \
	SDEI_EXPLICIT_EVENT(5000, SDEI_MAPF_NORMAL), \
	SDEI_EXPLICIT_EVENT(5001, SDEI_MAPF_NORMAL), \
	SDEI_EXPLICIT_EVENT(5002, SDEI_MAPF_NORMAL), \
	SDEI_EXPLICIT_EVENT(5003, SDEI_MAPF_CRITICAL), \
	SDEI_EXPLICIT_EVENT(5004, SDEI_MAPF_CRITICAL)
  #else
  #define PLAT_ARM_PRIVATE_SDEI_EVENTS	ARM_SDEI_PRIVATE_EVENTS
  #endif
#define PLAT_ARM_SHARED_SDEI_EVENTS	ARM_SDEI_SHARED_EVENTS
#endif

#define PLAT_ARM_SP_IMAGE_STACK_BASE	(PLAT_SP_IMAGE_NS_BUF_BASE +	\
					 PLAT_SP_IMAGE_NS_BUF_SIZE)

#define PLAT_SP_PRI			0x20

/*
 * Physical and virtual address space limits for MMU in AARCH64 & AARCH32 modes
 */
#ifdef __aarch64__
#define PLAT_PHY_ADDR_SPACE_SIZE	(1ULL << 36)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ULL << 36)
#else
#define PLAT_PHY_ADDR_SPACE_SIZE	(1ULL << 32)
#define PLAT_VIRT_ADDR_SPACE_SIZE	(1ULL << 32)
#endif

/*
 * Maximum size of Event Log buffer used in Measured Boot Event Log driver
 * TODO: calculate maximum EventLog size using the calculation:
 * Maximum size of Event Log * Number of images
 */
#if (defined(SPD_spmd)) || (ENABLE_RME && (defined(SPD_tspd) || defined(SPD_opteed)))
/*
 * Account for additional measurements of secure partitions and SPM.
 * Also, account for OP-TEE running with maximum number of SPs.
 */
#define PLAT_ARM_EVENT_LOG_MAX_SIZE		UL(0x800)
#elif defined(IMAGE_BL1) && TRANSFER_LIST
#define PLAT_ARM_EVENT_LOG_MAX_SIZE		UL(0x200)
#else
#define PLAT_ARM_EVENT_LOG_MAX_SIZE		UL(0x400)
#endif

/*
 * Maximum size of Event Log buffer used for DRTM
 */
#define PLAT_DRTM_EVENT_LOG_MAX_SIZE		UL(0x300)

/*
 * Number of MMAP entries used by DRTM implementation
 */
#define PLAT_DRTM_MMAP_ENTRIES			PLAT_ARM_MMAP_ENTRIES

#endif /* PLATFORM_DEF_H */
