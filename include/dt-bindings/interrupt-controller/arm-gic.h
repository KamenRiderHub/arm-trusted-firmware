/*
 * Copyright (c) 2019-2021, Arm Limited and Contributors. All rights reserved.
 *
 * SPDX-License-Identifier: MIT
 *
 * This header provides constants for the ARM GIC.
 */

#ifndef _DT_BINDINGS_INTERRUPT_CONTROLLER_ARM_GIC_H
#define _DT_BINDINGS_INTERRUPT_CONTROLLER_ARM_GIC_H

#include <dt-bindings/interrupt-controller/irq.h>

/*在gic定义里，@fvp-base-gicv3.dtsi，interrupt (specifier) cell至少为3，每个cell 有不同定义

@https://www.kernel.org/doc/Documentation/devicetree/bindings/interrupt-controller/arm%2Cgic-v3.txt
#interrupt-cells : Specifies the number of cells needed to encode an
  interrupt source. Must be a single cell with a value of at least 3.
  If the system requires describing PPI affinity, then the value must
  be at least 4.

  The 1st cell is the interrupt type; 0 for SPI interrupts, 1 for PPI
  interrupts. Other values are reserved for future use.

  The 2nd cell contains the interrupt number for the interrupt type.
  SPI interrupts are in the range [0-987]. PPI interrupts are in the
  range [0-15].

  The 3rd cell is the flags, encoded as follows:
	bits[3:0] trigger type and level flags.
		1 = edge triggered
		4 = level triggered

  The 4th cell is a phandle to a node describing a set of CPUs this
  interrupt is affine to. The interrupt must be a PPI, and the node
  pointed must be a subnode of the "ppi-partitions" subnode. For
  interrupt types other than PPI or PPIs that are not partitionned,
  this cell must be zero. See the "ppi-partitions" node description
  below.

下面的GIC_CPU_MASK_RAW 等宏不是标准规范中的内容，而是 Linux 平台扩展定义，用于在 flags 字段中编码 CPU 路由信息。
在标准 GIC 中断 specifier 中，一般结构是固定为 3 个 cell（type, number, flags）。其中的 flags（第 3 个 cell）
只需要包含触发类型即可，但平台可以在此基础上扩展。
*/
/* interrupt specifier cell 0 */

#define GIC_SPI 0
#define GIC_PPI 1

/*
 * Interrupt specifier cell 2.
 * The flags in irq.h are valid, plus those below.
 */
#define GIC_CPU_MASK_RAW(x) ((x) << 8)
#define GIC_CPU_MASK_SIMPLE(num) GIC_CPU_MASK_RAW((1 << (num)) - 1)

#endif
