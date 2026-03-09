/*
 * arch/aarch64/include/asm/io.h
 *
 * Copyright (C) 2015 ARM Limited. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */
#ifndef __BWIO_H
#define __BWIO_H

#include <stdint.h>

#define UART_LSR_FIFOE		0x80	/* Fifo error */
#define UART_LSR_TEMT		0x40	/* Transmitter empty */
#define UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#define UART_LSR_BI		0x10	/* Break interrupt indicator */
#define UART_LSR_FE		0x08	/* Frame error indicator */
#define UART_LSR_PE		0x04	/* Parity error indicator */
#define UART_LSR_OE		0x02	/* Overrun error indicator */
#define UART_LSR_DR		0x01	/* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS	0x1E	/* BI, FE, PE, OE bits */

static inline void raw_writel(uint32_t val, void *addr)
{
	asm volatile ("str %w0, [%1]\n" : : "r" (val), "r" (addr));
}

static inline uint32_t raw_readl(void *addr)
{
	uint32_t val;

	asm volatile ("ldr %w0, [%1]\n" : "=r" (val) : "r" (addr));
	return val;
}


void print_char(char c);

void print_string(const char *str);

#define HEX_CHARS_PER_LONG	(2 * sizeof(long))
#define HEX_CHARS		"0123456789abcdef"

void print_ulong_hex(unsigned long val);
// 2^32 is 4,294,967,296
#define DEC_CHARS_PER_UINT	10

void print_uint_dec(unsigned int val);



#endif
