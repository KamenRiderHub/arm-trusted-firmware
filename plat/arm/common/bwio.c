/*
 * arch/aarch64/include/asm/io.h
 *
 * Copyright (C) 2015 ARM Limited. All rights reserved.
 *
 * Use of this source code is governed by a BSD-style license that can be
 * found in the LICENSE.txt file.
 */

#include <stdint.h>
#include <plat/arm/common/bwio.h>


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

void print_ulong_hex(unsigned long val)
{
	int i;

	for (i = HEX_CHARS_PER_LONG - 1; i >= 0; i--) {
		int v = (val >> (4 * i)) & 0xf;
		print_char(HEX_CHARS[v]);
	}
}


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