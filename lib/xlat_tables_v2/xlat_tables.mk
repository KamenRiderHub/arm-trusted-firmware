#
# Copyright (c) 2017-2020, Arm Limited and Contributors. All rights reserved.
#
# SPDX-License-Identifier: BSD-3-Clause
#

XLAT_TABLES_LIB_SRCS	:=	$(addprefix lib/xlat_tables_v2/,	\
				${ARCH}/enable_mmu.S			\
				${ARCH}/xlat_tables_arch.c		\
				xlat_tables_context.c			\
				xlat_tables_core.c			\
				xlat_tables_utils.c)

XLAT_TABLES_LIB_V2	:=	1
$(eval $(call add_define,XLAT_TABLES_LIB_V2))

# XLAT_TABLES_PRINT_ENTRY_ADDR: Optional flag to enable printing of descriptor
# slot addresses and raw descriptor values during translation table dumps.
# When enabled (set to 1), xlat_tables_print will include:
# - The memory address of each descriptor slot (&table_base[table_idx])
# - The raw descriptor value (desc)
# - For table descriptors, the next-level table base address (NEXT_TBL)
# Default: 0 (disabled)
XLAT_TABLES_PRINT_ENTRY_ADDR	?=	0
$(eval $(call add_define,XLAT_TABLES_PRINT_ENTRY_ADDR))

ifeq (${ALLOW_RO_XLAT_TABLES}, 1)
    include lib/xlat_tables_v2/ro_xlat_tables.mk
endif
