/*
 * platform.c - Platform initialization and I/O.
 *
 * Copyright (C) 2015 ARM Limited. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */


#include <stdint.h>
#include "mrzhang_uart.h"

#define UART_LSR_FIFOE		0x80	/* Fifo error */
#define UART_LSR_TEMT		0x40	/* Transmitter empty */
#define UART_LSR_THRE		0x20	/* Transmit-hold-register empty */
#define UART_LSR_BI		0x10	/* Break interrupt indicator */
#define UART_LSR_FE		0x08	/* Frame error indicator */
#define UART_LSR_PE		0x04	/* Parity error indicator */
#define UART_LSR_OE		0x02	/* Overrun error indicator */
#define UART_LSR_DR		0x01	/* Receiver data ready */
#define UART_LSR_BRK_ERROR_BITS	0x1E	/* BI, FE, PE, OE bits */


void print_char(char c)
{
	while ((raw_readl((void *)0x0C000014) & UART_LSR_THRE) == 0)
		;
	raw_writel(c,(void *)0x0C000000);
}

void print_string(const char *str)
{
	while (*str)
		print_char(*str++);
}

#define HEX_CHARS_PER_LONG	(2 * sizeof(long))
#define HEX_CHARS		"0123456789abcdef"

void print_ulong_hex(unsigned long val)
{
	int i;

	for (i = HEX_CHARS_PER_LONG - 1; i >= 0; i--) {
		int v = (val >> (4 * i)) & 0xf;
		print_char(HEX_CHARS[v]);
	}
}

// 2^32 is 4,294,967,296
#define DEC_CHARS_PER_UINT	10

void print_uint_dec(unsigned int val)
{
	char digits[DEC_CHARS_PER_UINT];
	int d = 0;

	do {
		digits[d] = val % 10;
		val /= 10;
		d++;
	} while (val);

	while (d--) {
		print_char('0' + digits[d]);
	}
}

void print_cpu_warn(unsigned int cpu, const char *str)
{
	print_string("CPU");
	print_uint_dec(cpu);
	print_string(" WARNING: ");
	print_string(str);
}

void print_cpu_msg(unsigned int cpu, const char *str)
{
	print_string("CPU");
	print_uint_dec(cpu);
	print_string(": ");
	print_string(str);
}

void init_uart(void)
{
	/*
	 * UART initialisation (38400 8N1)
	 */
	// raw_writel(0x10,	PL011(UARTIBRD));
	// raw_writel(0x0,		PL011(UARTFBRD));
	// /* Set parameters to 8N1 and enable the FIFOs */
	// raw_writel(0x70,	PL011(UART_LCR_H));
	// /* Enable the UART, TXen and RXen */
	// raw_writel(0x301,	PL011(UARTCR));

    raw_writel(0x80,(void *)0x0C00000C);
    raw_writel(0x36,(void *)0x0C000000);
    raw_writel(0x00,(void *)0x0C00000C);
    raw_writel(0x03,(void *)0x0C00000C);
    raw_writel(0x00,(void *)0x0C000008);
    raw_writel(0x02,(void *)0x0C000004);
	
}

void init_platform(void)
{
#ifdef SYSREGS_BASE
	/*
	 * CLCD output site MB
	 */
	raw_writel(0x0,		V2M_SYS(CFGDATA));
	/* START | WRITE | MUXFPGA | SITE_MB */
	raw_writel((1 << 31) | (1 << 30) | (7 << 20) | (0 << 16),
				V2M_SYS(CFGCTRL));
#endif
}
