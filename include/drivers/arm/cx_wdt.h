/*
    自研芯片看门狗
 */
#ifndef CX_WDT_H
#define CX_WDT_H

#include <lib/utils_def.h>
#include <stdint.h>

/* 寄存器偏移 */
#define CX_WDT_CR_OFF      0x00U  /* 控制寄存器 */
#define CX_WDT_TORR_OFF    0x04U  /* 超时范围寄存器 */
#define CX_WDT_CCVR_OFF    0x08U  /* 当前计数器值寄存器 */
#define CX_WDT_CRR_OFF     0x0CU  /* 计数器重启寄存器 */
#define CX_WDT_STAT_OFF    0x10U  /* 中断状态寄存器 */
#define CX_WDT_EOI_OFF     0x14U  /* 中断清除寄存器（读清除） */

/* 控制寄存器 */
#define CX_WDT_CR_EN       (1U << 0) // WDT使能位
#define CX_WDT_CR_RMOD     (1U << 1) // Response mode 响应模式选择，选择1以模仿原ATF代码的效果（第一次中断，第二次复位），相关的清中断效果与不可读写的NEW_RMOD=0有关
#define CX_WDT_CR_RPL      (0U << 2) // Reset pulse length 复位信号长度，可以自行设置，但这里还是用默认的值

/* 超时范围寄存器 */
#define CX_WDT_TORR_TOP_SHIFT       0
#define CX_WDT_TORR_TOP_INIT_SHIFT  4

/* 计数器重置寄存器 */
#define CX_WDT_CRR_RESTART  0x76U   // 写入0x76U，计数器会被重置为0，此数防止意外的重启，如写入其他值，计数器不会被重置

/* 顶级参数（只读） */
#define WDT_CNT_WIDTH      32U   /* 计数器位宽 = 32 位。决定了最大计数周期 (2^32 - 1) */

#define WDT_DFLT_TOP_INIT  0U    /* 上电复位后，TORR.TOP_INIT 默认值 (仅当 DUAL_TOP=1 有效) */

#define WDT_DFLT_TOP       0U    /* 上电复位后，TORR.TOP 默认值，= 2^(16+0) 个 PCLK 周期作为默认超时*/

#define WDT_USE_FIX_TOP    1U    /* 1: 使用预定义 TOP 表（2^16 到 2^(CNT_WIDTH-1)）； 0: 用户自定义范围 (需要 WDT_USER_TOP 参数) */

#define WDT_HC_TOP         0U    /* 0: TOP 可编程；1: TOP 硬编码不可改 */

#define WDT_HC_RPL         0U    /* 0: RPL 可编程WDT_CR.RPL；1: 硬编码 */

#define WDT_HC_RMOD        0U    /* 0: RMOD 可编程；1: 硬编码 */

#define WDT_DFLT_RMOD      0U    /* 响应模式的默认值：0=直接复位；1=中断+复位 */

#define WDT_NEW_RMOD       0U    /* 0: 如果中断清除，第二次超时才会复位；1：无论是否清中断，第二次超时始终都复位 */

#define WDT_DUAL_TOP       0U    /* 0: 不支持两个阶段，使用 TOP；1: 支持两个阶段，使用 TOP_INIT/TOP */

#define WDT_ALWAYS_EN      0U    /* 0: 软件可控使能；1: 硬件始终使能 */


/* API */
uint32_t cx_wdt_pick_top(uint32_t pclk_hz, uint32_t sec);
void cx_wdt_set_torr_dual_0(uintptr_t base, uint32_t pclk_hz, uint32_t sec);
void cx_wdt_start(uintptr_t base, uint32_t pclk_hz, uint32_t stage_timeout_sec);
void cx_wdt_stop(uintptr_t base);
void cx_wdt_refresh(uintptr_t base, uint32_t pclk_hz, uint32_t stage_timeout_sec);

#endif /* CX_WDT_H */
