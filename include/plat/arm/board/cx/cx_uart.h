#ifndef CX_UART_H
#define CX_UART_H

#define CLEAR 0
#define SET 1
#ifndef NULL
#define NULL ((void *) 0)
#endif
// UART 基址
#define CXUART_BASE           UL(0x0C000000)

// 寄存器偏移
#define CXUART_THR_OFFSET    0x00 // 发送 THR寄存器只有当DLAB bit (LCR[7]) 为0时才可访问
#define CXUART_RBR_OFFSET    0x00 // 接收 RBR寄存器只有当DLAB bit (LCR[7]) 为0时才可访问
#define CXUART_DLL_OFFSET    0x00 // 低位分频寄存器，仅在LCR[7]置1且串口不忙时才可访问
#define CXUART_DLH_OFFSET    0x04 // 高位分频寄存器，仅在LCR[7]置1且串口不忙时才可访问
#define CXUART_FCR_OFFSET    0x08 // FIFO控制寄存器
#define CXUART_IIR_OFFSET    0x08 // 中断识别寄存器，只读
#define CXUART_IER_OFFSET    0x04 // 中断使能寄存器，IER寄存器只有当DLAB bit (LCR[7]) 为0时才可访问
#define CXUART_LCR_OFFSET    0x0C // 行控制寄存器
#define CXUART_LSR_OFFSET    0x14 // 行状态寄存器，只读

// 掩码位
#define IER_IID_MASK          0x0f
// 控制位
#define LCR_DLAB_CTRL        (1u<<7) // 1 = 访问分频寄存器
#define LCR_WLS_8_CTRL       (3u<<0) // [1:0]=0b11 → 8 bit data
#define FCR_ENFIFO_CTRL      (1u<<0) // 开启FIFO
#define IER_EN_THREIRQ_CTRL  (1u<<1) // 开启FIFO THRE中断
//状态位
//LSR
#define LSR_TSR_EMPTY    (1u<<6)
#define LSR_THR_EMPTY       (1u<<5)
#define LSR_RX_DATA_RDBIT	(0)
#define LSR_RX_DATA_READY   (1u<< LSR_RX_DATA_RDBIT )
// //IIR
// #define IIR_THR_EMPTY       0x02
// #define IIR_RX_DATA_AVAIL   0x04
// #define IIR_BUSY_DETECT     0x07

// 波特率分频值（预置为 54 → ≈115200 @48MHz）
#define CXUART_DIV_LO 0x36 //低8位，54

#ifndef __ASSEMBLER__

#include <stdint.h>

/*
 * Initialize a new cxuart console instance and register it with the console
 * framework. The |console| pointer must point to storage that will be valid
 * for the lifetime of the console, such as a global or static local variable.
 * Its contents will be reinitialized from scratch.
 */
int console_cxuart_register(uintptr_t baseaddr, uint32_t clock, uint32_t baud,
			   console_t *console);
int console_cxuart_core_init(uintptr_t base_addr,unsigned int uart_clk, unsigned int baud_rate);
int console_cxuart_core_putc(int c, uintptr_t base_addr);
int console_cxuart_putc(int c, console_t *console);
int console_cxuart_core_getc(uintptr_t base_addr);
int console_cxuart_getc(console_t *console);
void console_cxuart_core_flush(uintptr_t base_addr);
void console_cxuart_flush(console_t *console);

// void print_string(const char *str);
void console_cxuart_putc_string(const char *str);
void print_uint32(uint32_t val);
#endif /*__ASSEMBLER__*/

#endif /* CX_UART_H */
