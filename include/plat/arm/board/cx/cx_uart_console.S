/*
 * Copyright (c) 2013-2020, ARM Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include <arch.h>
#include <asm_macros.S>
#include <assert_macros.S>
#include <console_macros.S>

/* 【改】将本文件内的“bc.xx”改为“bxx”（b.ne和b.eq） */
#include "cx_uart.h"

	/*
	 * "core" functions are low-level implementations that don't require
	 * writable memory and are thus safe to call in BL1 crash context.
	 */
	.globl console_cxuart_core_init
	.globl console_cxuart_core_putc
	.globl console_cxuart_core_getc
	.globl console_cxuart_core_flush

	.globl	console_cxuart_putc
	.globl	console_cxuart_getc
	.globl	console_cxuart_flush

	/* -----------------------------------------------
	 * int console_cxuart_core_init(uintptr_t base_addr,
	 * unsigned int uart_clk, unsigned int baud_rate)
	 * Function to initialize the console without a
	 * C Runtime to print debug information. This
	 * function will be accessed by console_init and
	 * crash reporting.
	 * In: x0 - console base address
	 *     w1 - Uart clock in Hz
	 *     w2 - Baud rate
	 * Out: return 1 on success else 0 on error
	 * Clobber list : x1, x2, x3, x4
	 * -----------------------------------------------
	 */
func console_cxuart_core_init
	/* Check the input base address */
	cbz	x0, core_init_fail//串口地址不能为0
	
	/* Check baud rate and uart clock for sanity */
	cbz	w1, core_init_fail//时钟频率不能为0
	cbz	w2, core_init_fail//波特率不能为0
	
	/* compute the divisor */
	/* Divisor =  Uart clock / (16 * baudrate) */
	lsl	w2, w2, #4
	udiv	w2, w1, w2
	and	w1, w2, #0xff		/* w1 = DLL */
	lsr	w2, w2, #8
	and	w2, w2, #0xff		/* w2 = DLLM */

	/* 允许访问分频寄存器 */
	ldr	w3, [x0, #CXUART_LCR_OFFSET] //从内存LCR中读出数据到寄存器
	orr	w3, w3, #LCR_DLAB_CTRL //w3 = w3 | LCR_DLAB_CTRL 将DLAB置1，允许访问分频寄存器
	str	w3, [x0, #CXUART_LCR_OFFSET] //将控制值写回LCR内存

	/* Program the baudrate */
	/* 向分频寄存器写入除数 */
	str	w1, [x0, #CXUART_DLL_OFFSET]	/* program DLL */
	str	w2, [x0, #CXUART_DLH_OFFSET]	/* program DLLM */
	
	/* 禁止访问分频寄存器 */
	mov w2,#~LCR_DLAB_CTRL
	and	w3, w3, w2
	str	w3, [x0, #CXUART_LCR_OFFSET] //将控制值写回LCR内存
	
	/* 设置数据长度为8位 */
	mov w3, #LCR_WLS_8_CTRL //设置数据长度为8位
	str	w3, [x0, #CXUART_LCR_OFFSET] //将控制值写回LCR内存

	/* no interrupt */
	mov	w3, #0
	str	w3, [x0, #CXUART_IER_OFFSET]
	/* 开启FIFO */
	//ldr	w3, [x0, #CXUART_FCR_OFFSET] //从内存FCR中读出数据到寄存器
	//orr w3, w3, #FCR_ENFIFO_CTRL //开启FIFO
	//str	w3, [x0, #CXUART_FCR_OFFSET] //将控制值写回FCR内存

	/* 开启FIFO中断 */
	//ldr	w3, [x0, #CXUART_IER_OFFSET] //从内存IER中读出数据到寄存器
	//orr w3, w3, #IER_EN_THREIRQ_CTRL //开启FIFO THRE中断
	//str	w3, [x0, #CXUART_IER_OFFSET] //将控制值写回IER内存

	mov	w0, #1 //返回值会返回w0里的参数，因此这里要先往w0内写入1——表示成功
	ret
core_init_fail:
	mov	w0, #0 //如果失败，则返回0
	ret
endfunc console_cxuart_core_init

	.globl console_cxuart_register

	/* -----------------------------------------------
	 * int console_cxuart_register(uintptr_t baseaddr,
	 *     uint32_t clock, uint32_t baud,
	 *     console_t *console);
	 * Function to initialize and register a new cxuart
	 * console. Storage passed in for the console struct
	 * *must* be persistent (i.e. not from the stack).
	 * In: x0 - UART register base address
	 *     w1 - UART clock in Hz
	 *     w2 - Baud rate
	 *     x3 - pointer to empty console_t struct
	 * Out: return 1 on success, 0 on error
	 * Clobber list : x0, x1, x2, x6, x7, x14
	 * -----------------------------------------------
	 */
func console_cxuart_register
	mov	x7, x30 //将返回地址保存到x7
	mov	x6, x3 //将控制台地址保存到x6
	cbz	x6, register_fail //如果x6为0，则跳转到失败处理
	str	x0, [x6, #CONSOLE_T_BASE]//将串口基地址写入控制台基地址

	bl	console_cxuart_core_init
	cbz	x0, register_fail

	mov	x0, x6
	mov	x30, x7
	finish_console_register cxuart putc=1, getc=ENABLE_CONSOLE_GETC, flush=1 //把putc getc flush这三个回调函数给注册了

register_fail:
	ret	x7
endfunc console_cxuart_register

	/* --------------------------------------------------------
	 * int console_cxuart_core_putc(int c, uintptr_t base_addr)
	 * Function to output a character over the console. It
	 * returns the character printed on success or -1 on error.
	 * In : w0 - character to be printed
	 *      x1 - console base address
	 * Out : return -1 on error else return character.
	 * Clobber list : x2
	 * --------------------------------------------------------
	 */
func console_cxuart_core_putc
#if ENABLE_ASSERTIONS
	cmp	x1, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */

	/* Prepend '\r' to '\n' */
	cmp	w0, #0xA //检查待发送的字符是否为换行符
	b.ne	2f //如果不是换行符，则往前(f)跳转到标签2的位置。
1:
	/* Check if the transmit FIFO is empty */
	ldr	w2, [x1, #CXUART_LSR_OFFSET] //读取行状态寄存器的值
	and w2, w2, #(LSR_TSR_EMPTY | LSR_THR_EMPTY) //提取寄存器中THRE部分的值
	cmp w2, #(LSR_TSR_EMPTY | LSR_THR_EMPTY) //将寄存器中的值和参考值进行对比，判断THR是否为空
	b.ne 1b //若THR仍有数据，往回(b)跳转到标签1的位置
	mov	w2, #0xD //将回车符写入W2
	str	w2, [x1, #CXUART_THR_OFFSET] //发送回车符\r，然后继续执行标签2的代码（不是C语言的switch）
2:
	/* Check if the transmit FIFO is empty */
	ldr	w2, [x1, #CXUART_LSR_OFFSET] //读取行状态寄存器的值
	and w2, w2, #(LSR_TSR_EMPTY | LSR_THR_EMPTY) //提取寄存器中THRE部分的值
	cmp w2, #(LSR_TSR_EMPTY | LSR_THR_EMPTY) //将寄存器中的值和参考值进行对比，判断THR是否为空
	b.ne 2b //若THR仍有数据，往回(b)跳转到标签2的位置
	str	w0, [x1, #CXUART_THR_OFFSET] //若THR为空，则将字符送入THR里。
	ret
endfunc console_cxuart_core_putc

	/* --------------------------------------------------------
	 * int console_cxuart_putc(int c, console_t *console)
	 * Function to output a character over the console. It
	 * returns the character printed on success or -1 on error.
	 * In : w0 - character to be printed
	 *      x1 - pointer to console_t structure
	 * Out : return -1 on error else return character.
	 * Clobber list : x2
	 * --------------------------------------------------------
	 */
func console_cxuart_putc
#if ENABLE_ASSERTIONS
	cmp	x1, #0
	ASM_ASSERT(ne)//x1不能为0
#endif /* ENABLE_ASSERTIONS */
	ldr	x1, [x1, #CONSOLE_T_BASE] //从控制台基地址处读出串口基地址至x1，给下面的函数用
	b	console_cxuart_core_putc //无返回无条件跳转至console_cxuart_core_putc
endfunc console_cxuart_putc

	/* ---------------------------------------------
	 * int console_cxuart_core_getc(uintptr_t base_addr)
	 * Function to get a character from the console.
	 * It returns the character grabbed on success
	 * or -1 if no character is available.
	 * In : x0 - console base address
	 * Out: w0 - character if available, else -1
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
func console_cxuart_core_getc
#if ENABLE_ASSERTIONS
	cmp	x0, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */

	/* Check if the receive FIFO is empty */
	ldr	w1, [x0, #CXUART_LSR_OFFSET]//读取行状态寄存器
	tbz	w1, #LSR_RX_DATA_RDBIT, no_char //将寄存器中的值和参考值进行对比，判断是否有接收数据，若没有数据，则跳转到no_char的位置
	ldr	w0, [x0, #CXUART_RBR_OFFSET] //从RBR中读取数据
	ret
no_char:
	mov	w0, #ERROR_NO_PENDING_CHAR //把错误信息存到w0
	ret
endfunc console_cxuart_core_getc

	/* ---------------------------------------------
	 * int console_cxuart_getc(console_t *console)
	 * Function to get a character from the console.
	 * It returns the character grabbed on success
	 * or -1 if no character is available.
	 * In : x0 - pointer to console_t structure
	 * Out: w0 - character if available, else -1
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
func console_cxuart_getc
#if ENABLE_ASSERTIONS
	cmp	x0, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */
	ldr	x0, [x0, #CONSOLE_T_BASE] //把串口基地址存到x0
	b	console_cxuart_core_getc
endfunc console_cxuart_getc

	/* ---------------------------------------------
	 * void console_cxuart_core_flush(uintptr_t base_addr)
	 * Function to force a write of all buffered
	 * data that hasn't been output.
	 * In : x0 - console base address
	 * Out : void.
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
func console_cxuart_core_flush
#if ENABLE_ASSERTIONS
	cmp	x0, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */
1:
	/* Loop until the transmit FIFO is empty */
	ldr	w1, [x0, #CXUART_LSR_OFFSET]//读取行控制寄存器
	and w1, w1, #(LSR_TSR_EMPTY | LSR_THR_EMPTY) //提取寄存器中TEMT部分的值（THR和TSR同时为空）
	cmp w1, #(LSR_TSR_EMPTY | LSR_THR_EMPTY) //将寄存器中的值和参考值进行对比，判断所有数据是否已发出去
	b.ne 1b //若没有，则往回(b)跳转到标签1

	ret
endfunc console_cxuart_core_flush

	/* ---------------------------------------------
	 * void console_cxuart_flush(console_t *console)
	 * Function to force a write of all buffered
	 * data that hasn't been output.
	 * In : x0 - pointer to console_t structure
	 * Out : void
	 * Clobber list : x0, x1
	 * ---------------------------------------------
	 */
func console_cxuart_flush
#if ENABLE_ASSERTIONS
	cmp	x0, #0
	ASM_ASSERT(ne)
#endif /* ENABLE_ASSERTIONS */
	ldr	x0, [x0, #CONSOLE_T_BASE] //把串口基地址存到x0
	b	console_cxuart_core_flush
endfunc console_cxuart_flush
