#ifndef CX_CONSOLE_H
#define CX_CONSOLE_H

#include "cx_uart.h"


void __init cx_console_boot_init(void);
void cx_console_boot_end(void);
void cx_console_runtime_init(void);
void cx_console_runtime_end(void);
#include <plat/arm/common/bwio.h>

//【改】增加专门测试语句
void atf_testpoint_el3exit(void);

void atf_testpoint_savesp(void);

void atf_testpoint_exreturn(void);

void atf_testpoint_resptw(void);

void atf_testpoint_resgp(void);

void atf_testpoint_eret(void);

void atf_testpoint_bl2entry_S(void);

void atf_testpoint_printReg_hex(unsigned long info);

void atf_testpoint_printReg_bin(unsigned long info);

void atf_testpoint_printReg_udec(unsigned long info);

void atf_testpoint_x03(uint64_t t0, uint64_t t1, uint64_t t2, uint64_t t3);



void atf_testpoint_printZZ(void);

void atf_backup0_3(void);

void atf_recover0_3(void);

void atf_testpoint_test(void);

void atf_testpoint_test2(void);

void atf_testpoint_printAddVal(unsigned long info);

void atf_testpoint_printAddVal_double(unsigned long info);

void atf_testpoint_printAddVal_tri(unsigned long info);


void atf_testpoint_printMEM_32B(unsigned long info);

void atf_testpoint_print_security_state(unsigned long info);

void atf_testpoint_print_RegGroup1();

void atf_testpoint_print_WriteELR();

void atf_testpoint_print_RegGroup2();

void atf_testpoint_print_PC();

void atf_testpoint_print_RegGroup3();

void atf_testpoint_print_WriteELR_2();

void atf_testpoint_print_WriteESR();

void atf_testpoint_print_RegGroup3_in_BL2_EL1();

void atf_testpoint_el3_entrypoint(void);

void atf_scan_Memory_tmpAll();

void atf_scan_Memory_fast_mode();

void atf_scan_Memory_miniConfirm();

void atf_reg_test();

void atf_pcie_test();


#endif /* CX_CONSOLE_H */
