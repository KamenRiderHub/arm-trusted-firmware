/*
    自研芯片看门狗
    模仿ATF对sp805看门狗的使用逻辑，尤其是refresh，ATF用refresh是直接写个1 tick，让它几乎瞬间跑到复位。
    注意需要求改“include\plat\arm\common\arm_def.h”路径下的“ARM_SP805_TWDG_BASE、ARM_SP805_TWDG_CLK_HZ、ARM_TWDG_TIMEOUT_SEC、ARM_TWDG_LOAD_VAL”
    同时注意WDT_CNT_WIDTH = 32，且 WDT_USE_FIX_TOP = 1 → TOP ∈ [0..15]，这样限制了它的上限为2的31次方，假设频率位100M，那实际超时时间只能写到比16秒多一点
    以及需要把“plat\arm\board\fvp\fvp_bl1_setup.c”等路径下调用sp805的start、stop、refresh的地方都改成cx_wdt的

 */

#include <stdint.h>
//【注】TODO：头文件路径处理
#include <drivers/arm/cx_wdt.h>
#include <lib/mmio.h>


/* 选择TOP（超时周期挡位）：计算[0..15]中的最小TOP，使得2^(16+TOP)/pclk_hz >= sec */
uint32_t cx_wdt_pick_top(uint32_t pclk_hz, uint32_t sec)
{
    // 可能会溢出所以用long long
    unsigned long long cycles_needed = (unsigned long long)pclk_hz * (unsigned long long)sec;

    for (uint32_t top = 0; top <= 15U; ++top) 
    {
        unsigned long long cycles = 1ULL << (16U + top);
        if (cycles >= cycles_needed) 
        {
            return top;
        }
    }
    return 15U; // 最大上限
}

// 计算TOP挡位,只设置TORR寄存器的TOP字段
void cx_wdt_set_torr_dual_0(uintptr_t base, uint32_t pclk_hz, uint32_t sec)
{
    uint32_t top = cx_wdt_pick_top(pclk_hz, sec);
    uint32_t torr = (top << CX_WDT_TORR_TOP_SHIFT);
    mmio_write_32(base + CX_WDT_TORR_OFF, torr);
}

void cx_wdt_start(uintptr_t base, uint32_t pclk_hz, uint32_t stage_timeout_sec)
{
    /*
    调用函数计算超时挡位号，2^(16+TOP) == pclk_hz * 超时周期
    设置超时寄存器TORR的TOP字段，由于IP设置了WDT_DUAL_TOP为0，只有TOP字段有效
    */ 
    cx_wdt_set_torr_dual_0(base, pclk_hz, stage_timeout_sec);

    // 保险起见，清下中断
    (void)mmio_read_32(base + CX_WDT_EOI_OFF);

    // 配置控制寄存器（使能WDT，设置响应模式和复位脉冲长度）
    uint32_t cr = (CX_WDT_CR_EN | CX_WDT_CR_RMOD | CX_WDT_CR_RPL);
    mmio_write_32(base + CX_WDT_CR_OFF, cr);

    //向 WDT_CRR 写入固定值 0x76 来重启计数器,让计数器重新加载 TORR 里的超时值开始倒计时
    mmio_write_32(base + CX_WDT_CRR_OFF, CX_WDT_CRR_RESTART);

}

void cx_wdt_stop(uintptr_t base)
{
    //读取原本的设置
    uint32_t cr = mmio_read_32(base + CX_WDT_CR_OFF);
    // 清 EN bit
    cr &= ~CX_WDT_CR_EN;
    mmio_write_32(base + CX_WDT_CR_OFF, cr);
    // 保险起见，清下中断
    (void)mmio_read_32(base + CX_WDT_EOI_OFF);
}


void cx_wdt_refresh(uintptr_t base, uint32_t pclk_hz, uint32_t stage_timeout_sec)
{
    // 重新设置超时挡位号
    cx_wdt_set_torr_dual_0(base, pclk_hz, stage_timeout_sec);

    // // 保险起见，清下中断
    // (void)mmio_read_32(base + CX_WDT_EOI_OFF);

    // 重启计数器（写固定值 0x76U）
    mmio_write_32(base + CX_WDT_CRR_OFF, CX_WDT_CRR_RESTART);

}
