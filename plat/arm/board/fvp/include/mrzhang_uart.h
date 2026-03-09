#ifndef MRZHANG_UART_H
#define MRZHANG_UART_H

#include <stdint.h>

//*******************bits.h*******************
#ifdef __ASSEMBLY__
#define UL1(x)	x
#define ULL1(x)	x
#else
#define UL1(x)	x##UL
#define ULL1(x)	x##ULL
#endif

/*
 * Define a contiguous mask of bits with `msb` as the most significant bit and
 * `lsb` as the least significant bit. The `msb` value must be greater than or
 * equal to `lsb`.
 *
 * For example:
 * - BITS(63, 63) is 0x8000000000000000
 * - BITS(63, 0)  is 0xFFFFFFFFFFFFFFFF
 * - BITS(0, 0)   is 0x0000000000000001
 * - BITS(49, 17) is 0x0003FFFFFFFE0000
 */
#define BITSB(msb, lsb) \
	((~ULL1(0) >> (63 - msb)) & (~ULL1(0) << lsb))

/*
 * Define a mask of a single set bit `b`.
 *
 * For example:
 * - BIT(63) is 0x8000000000000000
 * - BIT(0)  is 0x0000000000000001
 * - BIT(32) is 0x0000000100000000
 */
#define BITB(b)	BITSB(b, b)

/*
 * Find the least significant set bit in the contiguous set of bits in `mask`.
 *
 * For example:
 * - BITS_LSB(0x0000000000000001) is 0
 * - BITS_LSB(0x000000000000ff00) is 8
 * - BITS_LSB(0x8000000000000000) is 63
 */
#define BITSB_LSB(mask)	(__builtin_ffsll(mask) - 1)

/*
 * Extract a bit-field out of `val` described by the contiguous set of bits in
 * `mask`.
 *
 * For example:
 * - BITS_EXTRACT(0xABCD, BITS(15, 12)) is 0xA
 * - BITS_EXTRACT(0xABCD, BITS(11, 8))  is 0xB
 * - BITS_EXTRACT(0xABCD, BIT(7))       is 0x1
 */
#define BITSB_EXTRACT(val, mask) \
	(((val) & (mask)) >> BITSB_LSB(mask))
//*******************bits.h*******************


//*******************cpu.h*******************
#define MPIDR_ID_BITS		0x00ffffff
#define MPIDR_INVALID		(-1)

#define ID_PFR1_GIC		BITSB(31, 28)

/* Only RES1 bits and CP15 barriers for the kernel */
#define HSCTLR_KERNEL		(3 << 28 | 3 << 22 | 1 << 18 | 1 << 16 | 1 << 11 | 3 << 4)
#define SCTLR_KERNEL		(3 << 22 | 1 << 11 | 1 << 5 | 3 << 4)

#define PSR_SVC			0x13
#define PSR_HYP			0x1a
#define PSR_MON			0x16
#define PSR_MODE_MASK		0x1f

#define PSR_T			(1 << 5)
#define PSR_F			(1 << 6)
#define PSR_I			(1 << 7)
#define PSR_A			(1 << 8)

#define SCR_NS			BITB(0)
#define SCR_HCE			BITB(8)

#define NSACR_CP10		BITB(10)
#define NSACR_CP11		BITB(11)

#define SPSR_KERNEL		(PSR_A | PSR_I | PSR_F | PSR_HYP)

#ifndef __ASSEMBLY__

#ifdef __ARM_ARCH_8A__
#define sevl()		asm volatile ("sevl" : : : "memory")
#else
/* sevl doesn't exist on ARMv7. Send event globally */
#define sevl()		asm volatile ("sev" : : : "memory")
#endif

static inline unsigned long read_cpsr(void)
{
	unsigned long cpsr;
	asm volatile ("mrs      %0, cpsr\n" : "=r" (cpsr));
	return cpsr;
}

#define read_cpsr_mode()       (read_cpsr() & PSR_MODE_MASK)

#define MPIDR		"p15, 0, %0, c0, c0, 5"
#define ID_PFR1		"p15, 0, %0, c0, c1, 1"
#define SCR		"p15, 0, %0, c1, c1, 0"
#define NSACR		"p15, 0, %0, c1, c1, 2"
#define ICIALLU		"p15, 0, %0, c7, c5, 0"
#define MVBAR		"p15, 0, %0, c12, c0, 1"

#define ICC_SRE		"p15, 6, %0, c12, c12, 5"
#define ICC_CTLR	"p15, 6, %0, c12, c12, 4"

#define CNTFRQ		"p15, 0, %0, c14, c0, 0"

#define mrc(reg)						\
({								\
	unsigned long __mrc_val;				\
	asm volatile("mrc " reg : "=r" (__mrc_val));		\
	__mrc_val;						\
})

#define mcr(reg, val)						\
do {								\
	unsigned long __mcr_val = val;				\
	asm volatile("mcr " reg : : "r" (__mcr_val));		\
} while (0)


#define mrc_field(reg, field) \
	BITSB_EXTRACT(mrc(reg), (reg##_##field))

static inline unsigned long read_mpidr1(void)
{
	return mrc(MPIDR) & MPIDR_ID_BITS;
}

static inline void iciallu(void)
{
	mcr(ICIALLU, 0);
}

static inline int has_gicv3_sysreg(void)
{
	return !!mrc_field(ID_PFR1, GIC);
}

#endif /* __ASSEMBLY__ */
//*******************cpu.h*******************


//*******************io.h*******************
#ifndef __ASSEMBLY__

static inline void raw_writel(uint32_t val, void *addr)
{
	asm volatile ("str %0, [%1]\n" : : "r" (val), "r" (addr));
}

static inline uint32_t raw_readl(void *addr)
{
	uint32_t val;

	asm volatile ("ldr %0, [%1]\n" : "=r" (val) : "r" (addr));
	return val;
}

#endif /* !__ASSEMBLY__ */
//*******************io.h*******************

//*******************platform.h*******************
void print_char(char c);
void print_string(const char *str);
void print_ulong_hex(unsigned long val);
void print_uint_dec(unsigned int val);

void print_cpu_warn(unsigned int cpu, const char *str);
void print_cpu_msg(unsigned int cpu, const char *str);

void init_uart(void);

void init_platform(void);
//*******************platform.h*******************

#endif