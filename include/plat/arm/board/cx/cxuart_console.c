#include <assert.h>
#include <lib/mmio.h>
#include "cx_uart.h"
#include "cx_platform_def.h"
#include "common/build_message.h"

void print_uint32(uint32_t val) {
    char buf[11];  // 10位十进制 + '\0'
    int i = 0;
    if(val == 0) {
        console_cxuart_core_putc('0', 0x0C000000);
        return;
    }
    while(val > 0) {
        buf[i++] = '0' + (val % 10);
        val /= 10;
    }
    // 逆序输出
    while(i--) {
        console_cxuart_core_putc(buf[i], 0x0C000000);
    }
}
void console_cxuart_putc_string(const char *str)
{
    while(*str)
        console_cxuart_core_putc(*str++, 0x0C000000);
}

    /* -----------------------------------------------
	 * int console_cxuart_core_init(uintptr_t base_addr,
	 * unsigned int uart_clk, unsigned int baud_rate)
	 * Function to initialize the console without a
	 * C Runtime to print debug information. This
	 * function will be accessed by console_init and
	 * crash reporting.
	 * In: base_addr - console base address
	 *     uart_clk - Uart clock in Hz
	 *     baud_rate - Baud rate
	 * Out: return 1 on success else 0 on error
	 * -----------------------------------------------
	 */
int console_cxuart_core_init(uintptr_t base_addr,unsigned int uart_clk, unsigned int baud_rate)
{
    unsigned int divisor;
    unsigned char DLL,DLH;
    // unsigned int LCR_temp,IER_temp,FCR_temp;
    unsigned int LCR_temp, FCR_temp;

    // 判断参数是否为0
    if((base_addr == 0)||(uart_clk == 0)||(baud_rate == 0))//若串口地址、时钟频率、波特率其中一个为0，则返回0，代表失败。
        return 0;
    
    //计算除数
    divisor = (uart_clk / (baud_rate << 4));
    DLL = (divisor & 0xff);
    DLH = (divisor >> 8) & 0xff;
    
    //允许访问分频寄存器
    LCR_temp = (mmio_read_32(base_addr + CXUART_LCR_OFFSET) | LCR_DLAB_CTRL);
    mmio_write_32(base_addr + CXUART_LCR_OFFSET, LCR_temp);

    //向分频寄存器写入除数
    mmio_write_32(base_addr + CXUART_DLL_OFFSET, DLL);
    mmio_write_32(base_addr + CXUART_DLH_OFFSET, DLH);

    //禁止访问分频寄存器
    LCR_temp = (mmio_read_32(base_addr + CXUART_LCR_OFFSET) & (~LCR_DLAB_CTRL));
    mmio_write_32(base_addr + CXUART_LCR_OFFSET, LCR_temp);
    
    //设置数据长度为8位
    LCR_temp = (mmio_read_32(base_addr + CXUART_LCR_OFFSET) | LCR_WLS_8_CTRL);
    mmio_write_32(base_addr + CXUART_LCR_OFFSET, LCR_temp);

    //开启FIFO中断
    // IER_temp = (mmio_read_32(base_addr + CXUART_IER_OFFSET) | IER_EN_THREIRQ_CTRL);
    // mmio_write_32(base_addr + CXUART_IER_OFFSET, IER_temp);

    //开启FIFO
    FCR_temp = (mmio_read_32(base_addr + CXUART_FCR_OFFSET) | FCR_ENFIFO_CTRL);
    mmio_write_32(base_addr + CXUART_FCR_OFFSET, FCR_temp);

    // print_string("This is UART!15.26\n\r");
    // console_cxuart_putc_string("This is UART! From putc_Driver\n\r");
    // console_cxuart_core_putc('A', 0x0C000000);
    // console_cxuart_core_putc('\n', 0x0C000000);
    // console_cxuart_putc_string(build_message);
	// console_cxuart_putc_string("\n");
    return 1;//表示成功
}

    /* -----------------------------------------------
	 * int console_cxuart_register(uintptr_t baseaddr,
	 *     uint32_t clock, uint32_t baud,
	 *     console_t *console);
	 * Function to initialize and register a new cxuart
	 * console. Storage passed in for the console struct
	 * *must* be persistent (i.e. not from the stack).
	 * In: base_addr - UART register base address
	 *     uart_clk - UART clock in Hz
	 *     baud_rate - Baud rate
	 *     *console - pointer to empty console_t struct
	 * Out: return 1 on success, 0 on error
	 * -----------------------------------------------
	 */
int console_cxuart_register(uintptr_t base_addr,uint32_t uart_clk, uint32_t baud_rate,console_t *console)
{
    //判断传进来的控制台地址是否合法
    if(console == NULL)
    {
        console_cxuart_putc_string("console_cxuart_register: console is NULL, failed!\n");
        return 0;
    }


    //将串口基地址写入控制台基地址
    console->base = base_addr;

    //配置串口,如果串口核心初始化传出的参数为0，则说明失败了，则返回0
    if (!console_cxuart_core_init(console->base, uart_clk, baud_rate))
        return 0;\

    //注册其他驱动函数
        console->putc = console_cxuart_putc;
    #if ENABLE_CONSOLE_GETC
        console->getc = console_cxuart_getc;
    #endif
        console->flush = console_cxuart_flush;
        console->flags = (CONSOLE_FLAG_BOOT | CONSOLE_FLAG_CRASH);

    //调用注册台注册函数
    return console_register(console);
}

    /* --------------------------------------------------------
	 * int console_cxuart_core_putc(int c, uintptr_t base_addr)
	 * Function to output a character over the console. It
	 * returns the character printed on success or -1 on error.
	 * In : c - character to be printed
	 *      base_addr - console base address
	 * Out : return -1 on error else return character.
	 * --------------------------------------------------------
	 */
int console_cxuart_core_putc(int c, uintptr_t base_addr)
{   

    //若识别到换行符，暂时不发，在换行符前面加一个\r回车符
    if(c == '\n')
    {
        console_cxuart_core_flush(base_addr);//等待之前的数据发完
        //先发送回车符\r
        mmio_write_32(base_addr + CXUART_THR_OFFSET, '\r');
    }
    
    console_cxuart_core_flush(base_addr);//等待之前的数据发完
    //发送字符c
    mmio_write_32(base_addr + CXUART_THR_OFFSET, c);

    return c;
}

    /* --------------------------------------------------------
	 * int console_cxuart_putc(int c, console_t *console)
	 * Function to output a character over the console. It
	 * returns the character printed on success or -1 on error.
	 * In : c - character to be printed
	 *      *console - pointer to console_t structure
	 * Out : return -1 on error else return character.
	 * --------------------------------------------------------
	 */
int console_cxuart_putc(int c, console_t *console)
{
    #if ENABLE_ASSERTIONS
	    assert(console->base != 0);
    #endif

    return console_cxuart_core_putc(c, console->base);
}

	/* ---------------------------------------------
	 * int console_cxuart_core_getc(uintptr_t base_addr)
	 * Function to get a character from the console.
	 * It returns the character grabbed on success
	 * or -1 if no character is available.
	 * In : base_addr - console base address
	 * Out: character if available, else -1
	 * ---------------------------------------------
	 */
int console_cxuart_core_getc(uintptr_t base_addr)
{
    unsigned int LSR_temp;

    LSR_temp = mmio_read_32(base_addr + CXUART_LSR_OFFSET);//读取行状态寄存器
    
    //将寄存器中的值和参考值进行对比，判断是否有接收数据，若没有数据，则返回-1
    if((LSR_temp & LSR_RX_DATA_READY) != LSR_RX_DATA_READY)
        return -1;
    //返回接收到的数据
    return mmio_read_32(base_addr + CXUART_RBR_OFFSET);
}

    /* ---------------------------------------------
	 * int console_cxuart_getc(console_t *console)
	 * Function to get a character from the console.
	 * It returns the character grabbed on success
	 * or -1 if no character is available.
	 * In : *console - pointer to console_t structure
	 * Out: character if available, else -1
	 * ---------------------------------------------
	 */
int console_cxuart_getc(console_t *console)
{
    #if ENABLE_ASSERTIONS
	    assert(console->base != 0);
    #endif
    
    return console_cxuart_core_getc(console->base);
}

    /* ---------------------------------------------
	 * void console_cxuart_core_flush(uintptr_t base_addr)
	 * Function to force a write of all buffered
	 * data that hasn't been output.
	 * In : base_addr - console base address
	 * Out : void.
	 * ---------------------------------------------
	 */
void console_cxuart_core_flush(uintptr_t base_addr)
{
    unsigned int LSR_temp;
    
    LSR_temp = ((mmio_read_32(base_addr + CXUART_LSR_OFFSET)) & (LSR_TSR_EMPTY | LSR_THR_EMPTY));//提取寄存器中THRE和TSRE部分的值
    while(LSR_temp != (LSR_TSR_EMPTY | LSR_THR_EMPTY))//将寄存器中的值和参考值进行对比，判断THR是否为空,若仍有数据，则等待
        LSR_temp = ((mmio_read_32(base_addr + CXUART_LSR_OFFSET)) & (LSR_TSR_EMPTY | LSR_THR_EMPTY));
}

    /* ---------------------------------------------
	 * void console_cxuart_flush(console_t *console)
	 * Function to force a write of all buffered
	 * data that hasn't been output.
	 * In : *console - pointer to console_t structure
	 * Out : void
	 * ---------------------------------------------
	 */
void console_cxuart_flush(console_t *console)
{
    #if ENABLE_ASSERTIONS
	    assert(console->base != 0);
    #endif

    console_cxuart_core_flush(console->base);
}

//【改】增加专门测试语句
void atf_testpoint_el3exit(void)
{
    printf("ATF_BP: Testpoint in el3_exit in [context.S]\n");
}

void atf_testpoint_savesp(void)
{
    printf("ATF_BP: Save the current SP_EL0 in el3_exit in [context.S]\n");
}

void atf_testpoint_exreturn(void)
{
    printf("ATF_BP: Apply exception_return in el3_exit in [context.S]\n");
}

void atf_testpoint_resptw(void)
{
    printf("ATF_BP: Apply restore_ptw_el1_sys_regs in el3_exit in [context.S]\n");
}

void atf_testpoint_resgp(void)
{
    printf("ATF_BP: Apply restore_gp_pmcr_pauth_regs in el3_exit in [context.S]\n");
}

void atf_testpoint_eret(void)
{
    printf("ATF_BP: Apply eret in exception_return in [asm_macro.S]\n");
}

void atf_testpoint_bl2entry_S(void)
{
    printf("ATF_BP: Testpoint in [bl2_entrypoint.S]\n");
}

void atf_testpoint_printReg_hex(unsigned long info)
{
    printf("ATF_BP: [Info-hex] 0x%lx \n",info);
}

void atf_testpoint_printReg_bin(unsigned long info)
{
    printf("ATF_BP: [Info-bin]");

    unsigned long mask = 1UL << 63;  // 从最高位开始
    for (int i = 0; i < 64; i++) 
    {
        printf("%c", (info & mask) ? '1' : '0');
        mask >>= 1;  // 右移一位
        if ((i + 1) % 8 == 0 && i < 31) {
            printf(" ");  // 每 8 位添加一个空格，提高可读性
        }
    }
    printf(" in [bl2_entrypoint.S]\n");
}

void atf_testpoint_printReg_udec(unsigned long info)
{
    printf("ATF_BP: [Info-udec] %lu in [bl2_entrypoint.S]\n",info);
}

void atf_testpoint_x03(uint64_t t0, uint64_t t1, uint64_t t2, uint64_t t3)
{
    printf("ATF_BP: x0 = %lx\n", t0);
    printf("ATF_BP: x1 = %lx\n", t1);
    printf("ATF_BP: x2 = %lx\n", t2);
    printf("ATF_BP: x3 = %lx\n", t3);
}

#include <plat/arm/common/bwio.h>

void atf_testpoint_printZZ(void)
{
    print_string("BL2 is here\r\n");
}


void atf_backup0_3(void)
{
    asm volatile (
        "mov    x11, x0\n"
        "mov    x12, x1\n"
        "mov    x13, x2\n"
        "mov    x14, x3\n"
    );
}


void atf_recover0_3(void)
{
    asm volatile (
        "mov    x0, x11\n"
        "mov    x1, x12\n"
        "mov    x2, x13\n"
        "mov    x3, x14\n"
    );
}

void atf_testpoint_test(void)
{
    print_string("ATF_BP: TEST-POINT\r\n");
}

void atf_testpoint_test2(void)
{
    printf("ATF_BP: TEST-POINT_2\n");
}

void atf_testpoint_printAddVal(unsigned long info)
{
    unsigned long value_at_info = *(unsigned long *)info;
    printf("ATF_BP: Value on 0x%lx is {0x%lx}\n",info,value_at_info);
}

void atf_testpoint_printAddVal_double(unsigned long info)
{
    unsigned long value_at_info = *(unsigned long *)info;
    unsigned long value_at_info2 = *(unsigned long *)value_at_info;

    printf("ATF_BP: Value on 0x%lx is {0x%lx}\n",value_at_info,value_at_info2);
}

void atf_testpoint_printAddVal_tri(unsigned long info)
{
    unsigned long value_at_info = *(unsigned long *)info;
    unsigned long value_at_info2 = *(unsigned long *)value_at_info;
    unsigned long value_at_info3 = *(unsigned long *)value_at_info2;


    printf("ATF_BP: Value on 0x%lx is {0x%lx}\n",value_at_info2,value_at_info3);
}

void atf_testpoint_printMEM_32B(unsigned long info)
{
    printf("ATF_BP: -------------------- [Print Memory] --------------------\n");
    printf("ATF_BP: Read 32 bytes from buffer 0x%lx:\n", info);
	printf("ATF_BP: Hex dump: ");
	for (int i = 0; i < 32; i++) {
		// 以十六进制打印每个字节，不足2位补0
		printf("%02x ", ((unsigned char *)info)[i]);
		// 每16字节换行，提高可读性
		if (((i + 1) % 16 == 0)&&(i!=31)) {
			printf("\nATF_BP: Hex dump: ");
		}
	}
	printf("\n");  // 结束当前行
    printf("ATF_BP: -------------------- [Print Memory] --------------------\n");

}
#include <export/common/ep_info_exp.h>
#include <plat/common/platform.h>
#include <bl31/bl31.h>

void atf_testpoint_print_security_state(unsigned long info)
{
	printf("ATF_BP: security_state is %lx\n",GET_SECURITY_STATE((bl31_plat_get_next_image_ep_info(bl31_get_next_image_type()))->h.attr));

}

void atf_testpoint_print_RegGroup1()
{
    printf("ATF_BP: ------------- [Print RegGroup1] -------------\n");
    printf("ATF_BP: CurrentEL = %lu\n", GET_EL(read_CurrentEl()));
	printf("ATF_BP: SP_EL1 = 0x%lx\n", read_sp_el1());
	printf("ATF_BP: SP_EL2 = 0x%lx\n", read_sp_el2());
	printf("ATF_BP: ELR_EL3 = 0x%lx\n", read_elr_el3());
	printf("ATF_BP: ELR_EL2 = 0x%lx\n", read_elr_el2());
	printf("ATF_BP: ELR_EL1 = 0x%lx\n", read_elr_el1());
	printf("ATF_BP: SPSR_EL3 = 0x%lx\n", read_spsr_el3());
	printf("ATF_BP: SPSR_EL2 = 0x%lx\n", read_spsr_el2());
	printf("ATF_BP: SPSR_EL1 = 0x%lx\n", read_spsr_el1());
    printf("ATF_BP: ------------- [Print RegGroup1] -------------\n");

}

void atf_testpoint_print_WriteELR()
{
    write_elr_el3(0x88200000);
    write_elr_el2(0x88200000);
    write_elr_el1(0x88200000);
}

void atf_testpoint_print_RegGroup2()
{
    printf("ATF_BP: ------------- [Print RegGroup2] -------------\n");
    printf("ATF_BP: CurrentEL = %lu\n", GET_EL(read_CurrentEl()));
	printf("ATF_BP: ELR_EL3 = 0x%lx\n", read_elr_el3());
	printf("ATF_BP: ELR_EL2 = 0x%lx\n", read_elr_el2());
	printf("ATF_BP: ELR_EL1 = 0x%lx\n", read_elr_el1());
    printf("ATF_BP: ------------- [Print RegGroup2] -------------\n");

}

void atf_testpoint_print_PC()
{
    printf("ATF_BP: ------------- [Print PC] -------------\n");
    asm volatile (
        "adr	x0, .\n"
        "bl    atf_testpoint_printReg_hex\n"
    );
}

void atf_testpoint_print_RegGroup3()
{
    printf("ATF_BP: ------------- [Print RegGroup3] -------------\n");

	printf("ATF_BP: ESR_EL3 = 0x%lx\n", read_esr_el3());
	printf("ATF_BP: ESR_EL2 = 0x%lx\n", read_esr_el2());
	printf("ATF_BP: ESR_EL1 = 0x%lx\n", read_esr_el1());
	printf("ATF_BP: FAR_EL3 = 0x%lx\n", read_far_el3());
	printf("ATF_BP: FAR_EL2 = 0x%lx\n", read_far_el2());
	printf("ATF_BP: FAR_EL1 = 0x%lx\n", read_far_el1());
	printf("ATF_BP: ELR_EL3 = 0x%lx\n", read_elr_el3());
	printf("ATF_BP: DAIF = 0x%lx\n", read_daif());
	printf("ATF_BP: DISR_EL1 = 0x%lx\n", read_disr_el1());
	printf("ATF_BP: ESR_EL3 = 0x%lx\n", read_esr_el3());
	printf("ATF_BP: SCR_EL3 = 0x%lx\n", read_scr_el3());

    printf("ATF_BP: ------------- [Print RegGroup3] -------------\n");
}

void atf_testpoint_print_WriteELR_2()
{
    write_elr_el3(0x824b90);
    write_elr_el2(0x824b90);
    write_elr_el1(0x824b90);
}

void atf_testpoint_print_WriteESR()
{
    write_esr_el3(0x0);
}

void atf_testpoint_print_RegGroup3_in_BL2_EL1()
{
    printf("ATF_BP: ------------- [Print RegGroup3_in_BL2_EL1] -------------\n");

	printf("ATF_BP: ESR_EL1 = 0x%lx\n", read_esr_el1());
	printf("ATF_BP: FAR_EL1 = 0x%lx\n", read_far_el1());
	printf("ATF_BP: DAIF = 0x%lx\n", read_daif());
	printf("ATF_BP: DISR_EL1 = 0x%lx\n", read_disr_el1());
	printf("ATF_BP: ESR_EL1 = 0x%lx\n", read_esr_el1());

    printf("ATF_BP: ------------- [Print RegGroup3_in_BL2_EL1] -------------\n");
}

void atf_testpoint_el3_entrypoint(void)
{
    print_string("ATF_BP: Testpoint in el3_entrypoint\r\n");
}


#include <lib/mmio.h>
// void atf_scan_Memory1()
// {
//     for(uint64_t i = 0x0;i<0x40000000;i+=0x1000)
//     {
//         mmio_read_64(i);
//         printf("0x%lx\n", i);
//     }
// }
// void atf_scan_Memory2()
// {
//     for(uint64_t i = 0x80000000;i<0xffffffff;i+=0x1000)
//     {
//         mmio_read_64(i);
//         printf("0x%lx\n", i);
//     }
// }

// void atf_scan_Memory3()
// {
//     for(uint64_t i = 0x900000000;i<0x380000000;i+=0x1000)
//     {
//         mmio_read_64(i);
//         printf("0x%lx\n", i);
//     }
// }

void atf_scan_Memory_QSPI()
{
    for(uint64_t i = 0;i<0x8000000;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
    }
}
void atf_scan_Memory_LPC()
{
    for(uint64_t i = 0x8000000;i<0xbffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
        // if(i>(0x8000000+0x10000))
        // {
        //     break;
        // }
    }
}
void atf_scan_Memory_device_part()
{
    for(uint64_t i = 0xc000000;i<0xcffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
        // if(i>(0xc000000+0x10000))
        // {
        //     break;
        // }
    }
}
void atf_scan_Memory_smmu()
{
    for(uint64_t i = 0xd000000;i<0xdffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
        // if(i>(0xd000000+0x10000))
        // {
        //     break;
        // }
    }
}
void atf_scan_Memory_gic()
{
    for(uint64_t i = 0xe000000;i<0xeffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
        // if(i>(0xe000000+0x10000))
        // {
        //     break;
        // }
    }
}
void atf_scan_Memory_sram()
{
    for(uint64_t i = 0xf000000;i<0xfffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
        // if(i>(0xf000000+0x10000))
        // {
        //     break;
        // }
    }
}
void atf_scan_Memory_dmc_periphbase()
{
    for(uint64_t i = 0x10000000;i<0x1fffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
        // if(i>(0x10000000+0x10000))
        // {
        //     break;
        // }
    }
}
void atf_scan_Memory_dsu_periphbase()
{
    for(uint64_t i = 0x20000000;i<0x2fffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
        // if(i>(0x20000000+0x10000))
        // {
        //     break;
        // }
    }
}
void atf_scan_Memory_cfgm_periphbase()
{
    for(uint64_t i = 0x30000000;i<0x3fffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
        // if(i>(0x30000000+0x10000))
        // {
        //     break;
        // }
    }
}
void atf_scan_Memory_pcie_ctrl()
{
    for(uint64_t i = 0x40000000;i<0x7fffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
    }
}
void atf_scan_Memory_space()
{
    for(uint64_t i = 0x80000000;i<0xffffffff;i+=0x10000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
    }
}
void atf_scan_Memory_space_new1()
{
    for(uint64_t i = 0xF7990000;i<=0xF7CB7680;i+=0x8)
    {
        mmio_read_64(i);
        mmio_write_64(i,0xAFAFAFAFAFAFAFAF);
        if(i%(0x10000)==0)
        {
            printf("[0x%lx - 0x%lx]\n", i-0x10000, i);
        }
        if(i==0xF7CB7680)
        {
            printf("[0x%x - 0x%x]\n", 0xF7CB0000, 0xF7CB7680);
        }
    }
}

void atf_scan_Memory_space_new2(uint64_t sta, uint64_t en)
{
    for(uint64_t i = sta;i<=en;i+=0x8)
    {
        mmio_read_64(i);
        mmio_write_64(i,0xAFAFAFAFAFAFAFAF);
        if(i%(0x100000)==0)
        {
            printf("[0x%lx - 0x%lx]\n", i-0x100000, i);
        }
        // if(i==0xF7CB7680)
        // {
        //     printf("[0x%x - 0x%x]\n", 0xF7CB0000, 0xF7CB7680);
        // }
        if(i==en)
        {
            printf("[END in %lx]\n", en);
        }
    }
}

//保证是对齐0x100000的情况下才能代替atf_scan_Memory_space_new2
void atf_scan_Memory_space_new3(uint64_t sta, uint64_t en)
{
    for(uint64_t i = sta;i<en;i+=0x100000)
    {
        for(uint64_t j=i;((j<i+0x100000)&&(j<en));j+=0x8)
        {
            mmio_read_64(i);
            mmio_write_64(i,0xAFAFAFAFAFAFAFAF);
        }
        printf("[0x%lx - 0x%lx]\n", i-0x100000, i);
    }
    printf("[END in %lx]\n", en);
}

void atf_scan_Memory_space_patternR(uint32_t sta, uint32_t en)
{
    uint32_t pattern1=0xAA55AA55;
    uint32_t pattern2=0x55AA55AA;
    uint32_t bef=0x0,aft=0x0;


    printf("TEST:0x%x - 0x%x\n", sta, en);
    // printf("Pattern 1: Value is 0xAA55AA55\n");
    for(uint32_t i = sta;i<=en;i+=0x4)
    {
        bef=mmio_read_32(i);
        mmio_write_32(i,pattern1);
        aft=mmio_read_32(i);
        if(aft!=pattern1)
        {
            printf("Mismatch(p1) in 0x%x - 0x%x,before is 0x%x\n", i, i+4,bef);
        }

        mmio_write_32(i,pattern2);
        aft=mmio_read_32(i);
        if(aft!=pattern2)
        {
            printf("Mismatch(p2) in 0x%x - 0x%x\n", i, i+4);
        }

        if(i%(0x10000)==0)
        {
            printf("[0x%x - 0x%x]\n", i-0x10000, i);
        }
        if(i==en)
        {
            printf("[END in %x]\n", en);
        }
    }
}

void atf_scan_Memory_pcie_m64()
{
    for(uint64_t i = 0x100000000;i<0x8ffffffff;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
    }
}
void atf_scan_Memory_space2_14G()
{
    for(uint64_t i = 0x900000000;i<0x380000000;i+=0x1000)
    {
        mmio_read_64(i);
        printf("0x%lx\n", i);
    }
}
void atf_scan_Memory_tmpAll()
{
    printf("ATF_BP: -------------------- [Scan Memory] --------------------\n");
    // atf_scan_Memory1();
    // printf("ATF_BP: -------------------- [Skip] --------------------\n");
    // atf_scan_Memory2();
    // printf("ATF_BP: -------------------- [Skip] --------------------\n");
    // atf_scan_Memory3();

    // atf_scan_Memory_QSPI();
    printf("ATF_BP: -------------------- [Skip-QSPI] --------------------\n");
    atf_scan_Memory_LPC();
    // atf_scan_Memory_device_part();
    printf("ATF_BP: -------------------- [Skip-device_part] --------------------\n");
    atf_scan_Memory_smmu();
    atf_scan_Memory_gic();
    atf_scan_Memory_sram();
    atf_scan_Memory_dmc_periphbase();
    atf_scan_Memory_dsu_periphbase();
    atf_scan_Memory_cfgm_periphbase();
    // atf_scan_Memory_pcie_ctrl();
    printf("ATF_BP: -------------------- [Skip-pcie_ctrl] --------------------\n");
    atf_scan_Memory_space();
    // atf_scan_Memory_pcie_m64();
    printf("ATF_BP: -------------------- [Skip-pcie_m64] --------------------\n");
    atf_scan_Memory_space2_14G();
    printf("ATF_BP: -------------------- [Scan Memory] --------------------\n");

}

void atf_scan_Memory_fast_mode()
{
    printf("ATF_BP: -------------------- [Scan Memory] --------------------\n");
    // atf_scan_Memory_space_new1();
    // atf_scan_Memory_space_new2(0x80000000,0xffffffff);
    atf_scan_Memory_space_patternR(0x88000000,0x88000000+0x08000000);
    printf("ATF_BP: -------------------- [Scan Memory] --------------------\n");
}
void atf_scan_Memory_miniConfirm()
{
    printf("ATF_BP: -------------------- [Scan Memory - miniConfirm] --------------------\n");
    uint32_t sta=0x88000000;
    uint32_t en=0x88000100;
    uint32_t pattern1=0x12345678;
    uint32_t pattern2=0x89abcdef;
    uint32_t aft=0x0;
    bool flag=true;
    // uint64_t aft_64=0x0;
    for(uint32_t i=sta;i<=en;i+=0x4)
    {
        printf("Addreess: i=%x\n", i);

        mmio_write_32(i,pattern1);
        aft=mmio_read_32(i);
        // aft_64=mmio_read_64(i);
        // printf("Pattern1 in 0x%x - 0x%x: 0x%x, read64=%lx\n", i, i+4,aft,aft_64);
        printf("Pattern1 in 0x%x - 0x%x: 0x%x\n", i, i+4,aft);

        mmio_write_32(i,pattern2);
        aft=mmio_read_32(i);
        // aft_64=mmio_read_64(i);
        // printf("Pattern2 in 0x%x - 0x%x: 0x%x, read64=%lx\n", i, i+4,aft,aft_64);
        printf("Pattern2 in 0x%x - 0x%x: 0x%x\n", i, i+4,aft);
        if(flag&&i!=sta)
        {
            printf("0x%x (64bit) is 0x%lx\n", i,mmio_read_64(i-0x8));
        }
        flag=!flag;
    }
    printf("ATF_BP: -------------------- [Scan Memory - miniConfirm] --------------------\n");
}


void atf_reg_test()
{
	printf(" --------------------- [Register Test] --------------------- \n");
	// uint32_t tmp_uart1 = mmio_read_32(0x0c001000 + CXUART_LCR_OFFSET);
	// printf(" Read 0x%x is 0x%x\n", 0x0c001000 + CXUART_LCR_OFFSET, tmp_uart1);
	// printf(" Write {0x%x}on 0x%x [UART1]\n", (tmp_uart1|LCR_DLAB_CTRL), 0x0c001000 + CXUART_LCR_OFFSET);
    // mmio_write_32(0x0c001000 + CXUART_LCR_OFFSET, (tmp_uart1|LCR_DLAB_CTRL));
	// printf(" Read 0x%x is 0x%x\n", 0x0c001000 + CXUART_LCR_OFFSET, mmio_read_32(0x0c001000 + CXUART_LCR_OFFSET));
    // mmio_write_32(0x0c001000 + CXUART_LCR_OFFSET, tmp_uart1);//恢复
	// printf(" [Recover] Read 0x%x is 0x%x\n", 0x0c001000 + CXUART_LCR_OFFSET, mmio_read_32(0x0c001000 + CXUART_LCR_OFFSET));
	
	// uint32_t tmp_uart1_2 = mmio_read_32(0x0c001000);
	
	// printf(" [Reverse]Write {0x%x} on 0x%x [UART1]\n", (~tmp_uart1_2), 0x0c001000);
    // mmio_write_32(0x0c001000, (~tmp_uart1_2));
	// printf(" [Reverse] Read 0x%x is 0x%x\n", 0x0c001000, mmio_read_32(0x0c001000));
    // mmio_write_32(0x0c001000, tmp_uart1_2);//恢复
    
	
	printf(" =========================================================== \n");
	
	//printf(" Write {0x12345678} on 0x0c001000 [UART1]\n");
	//mmio_write_32(0x0c001000, 0x12345678);
	//printf(" Read 0x0c001000 is 0x%x\n",mmio_read_32(0x0c001000));
	
	printf(" Read 0x0c004000 is 0x%x\n",mmio_read_8(0x0c004000));	
	printf(" Write {0x12} on 0x0c004000 [GPIO0]\n");
	mmio_write_8(0x0c004000, 0x12);
	printf(" Read 0x0c004000 is 0x%x\n",mmio_read_8(0x0c004000));
	printf(" =========================================================== \n");


	printf(" Read 0x0c005000 is 0x%x\n",mmio_read_8(0x0c005000));
	printf(" Write {0x12} on 0x0c005000 [GPIO1]\n");
	mmio_write_8(0x0c005000, 0x12);
	printf(" Read 0x0c005000 is 0x%x\n",mmio_read_8(0x0c005000));
	printf(" =========================================================== \n");

/*
	printf(" Write {0x12345678} on 0x0c004000 [GPIO0]\n");
	mmio_write_32(0x0c004000, 0x12345678);
	printf(" Read 0x0c004000 is 0x%x\n",mmio_read_32(0x0c004000));
	
	printf(" Write {0x12345678} on 0x0c005000 [GPIO1]\n");
	mmio_write_32(0x0c005000, 0x12345678);
	printf(" Read 0x0c005000 is 0x%x\n",mmio_read_32(0x0c005000));
*/

    printf(" Read 0x0c006000 is 0x%x\n",mmio_read_8(0x0c006000));
    printf(" Write {0x34} on 0x0c006000 [I2C0]\n");
    mmio_write_8(0x0c006000, 0x34);
    printf(" Read 0x0c006000 is 0x%x\n",mmio_read_8(0x0c006000));
	printf(" =========================================================== \n");

    printf(" Read 0x0c007000 is 0x%x\n",mmio_read_8(0x0c007000));
    printf(" Write {0x34} on 0x0c007000 [I2C1]\n");
    mmio_write_8(0x0c007000, 0x34);
    printf(" Read 0x0c007000 is 0x%x\n",mmio_read_8(0x0c007000));
	printf(" =========================================================== \n");

    printf(" Read 0x0c008000 is 0x%x\n",mmio_read_8(0x0c008000));
    printf(" Write {0x34} on 0x0c008000 [I2C2]\n");
    mmio_write_8(0x0c008000, 0x34);
    printf(" Read 0x0c008000 is 0x%x\n",mmio_read_8(0x0c008000));
	printf(" =========================================================== \n");

/*
    printf(" Write {0x12345678} on 0x0c006000 [I2C0]\n");
    mmio_write_32(0x0c006000, 0x12345678);
    printf(" Read 0x0c006000 is 0x%x\n",mmio_read_32(0x0c006000));

    printf(" Write {0x12345678} on 0x0c007000 [I2C1]\n");
    mmio_write_32(0x0c007000, 0x12345678);
    printf(" Read 0x0c007000 is 0x%x\n",mmio_read_32(0x0c007000));

    printf(" Write {0x12345678} on 0x0c008000 [I2C2]\n");
    mmio_write_32(0x0c008000, 0x12345678);
    printf(" Read 0x0c008000 is 0x%x\n",mmio_read_32(0x0c008000));
*/
    // printf(" [NO I3C]\n");
    // //mmio_write_32(0x0c009000, 0x12345678);
    // //printf(" Read 0x0c009000 is 0x%x\n",mmio_read_32(0x0c009000));
	// printf(" =========================================================== \n");

    printf(" Read 0x0c00a000 is 0x%x\n",mmio_read_8(0x0c00a000));
    uint8_t tmp_wdt=mmio_read_8(0x0c00a000);
    printf(" Write {0x%x} on 0x0c00a000 [WDT]\n",(tmp_wdt|(1<<5)));
    mmio_write_8(0x0c00a000, (tmp_wdt|(1<<5)));
    printf(" Read 0x0c00a000 is 0x%x\n",mmio_read_8(0x0c00a000));
	printf(" =========================================================== \n");

    //1070000
    uint32_t tmp_spi = mmio_read_32(0x0c00c000);
    printf(" Read 0x0c00c000 is 0x%x\n",tmp_spi);
    printf(" Write {0x%x} on 0x0c00c000 [SPI]\n",(tmp_spi|0xbc0));
    // mmio_write_32(0x0c00c000, (~tmp_spi));
    mmio_write_32(0x0c00c000, (tmp_spi|0xbc0));

    printf(" Read 0x0c00c000 is 0x%x\n",mmio_read_32(0x0c00c000));
    mmio_write_32(0x0c00c000, tmp_spi); //恢复
    printf(" [Recover] Read 0x0c00c000 is 0x%x\n",mmio_read_32(0x0c00c000));   
	printf(" =========================================================== \n");

    printf(" Read 0x0c00d000 is 0x%x\n",mmio_read_32(0x0c00d000));
    printf(" Write {0x12345678} on 0x0c00d000 [timer]\n");
    mmio_write_32(0x0c00d000, 0x12345678);
    printf(" Read 0x0c00d000 is 0x%x\n",mmio_read_32(0x0c00d000));
	printf(" =========================================================== \n");

    // uint32_t tmp_qspi = mmio_read_32(0x0c014000);
    // printf(" Read 0x0c014000 is 0x%x\n",tmp_qspi);
    // printf(" Write {0x%x} on 0x0c014000 [QSPI]\n",(~tmp_qspi));
    // mmio_write_32(0x0c014000, (~tmp_qspi));
    // printf(" Read 0x0c014000 is 0x%x\n",mmio_read_32(0x0c014000));
    // mmio_write_32(0x0c014000, tmp_qspi); //恢复
    // printf(" [Recover] Read 0x0c014000 is 0x%x\n",mmio_read_32(0x0c014000));       
	// printf(" =========================================================== \n");
 
 
    // uint32_t tmp_smmu = mmio_read_32(0x0d000044);
    // printf(" Read 0x0d000044 is 0x%x\n",tmp_smmu);
    // printf(" Write {0x%x} on 0x0d000044 [SMMU]\n", ((tmp_smmu)|(1U << 31)|(1U << 20)));
    // mmio_write_32(0x0d000044, ((tmp_smmu)|(1U << 31)|(1U << 20)));
    // printf(" Read 0x0d000044 is 0x%x\n",mmio_read_32(0x0d000044));   
    // mmio_write_32(0x0d000044, tmp_smmu); //恢复
    // printf(" [Recover] Read 0x0d000044 is 0x%x\n",mmio_read_32(0x0d000044));   
    
	// printf(" =========================================================== \n");
    
 
    uint32_t tmp_gic = mmio_read_32(0x0e000000);
    printf(" Read 0x0e000000 is 0x%x\n",tmp_gic);
    printf(" Write {0x%x} on 0x0e000000 [GIC]\n", ((tmp_gic)|(1U << 1)));
    // mmio_write_32(0x0e000000, ((tmp_gic)|(3U << 28)));
    mmio_write_32(0x0e000000, ((tmp_gic)|(1U << 1)));

    printf(" Read 0x0e000000 is 0x%x\n",mmio_read_32(0x0e000000));   
    mmio_write_32(0x0e000000, tmp_gic); //恢复
    printf(" [Recover] Read 0x0e000000 is 0x%x\n",mmio_read_32(0x0e000000));   
    
	printf(" =========================================================== \n");
 
    printf(" Read 0x0f000000 is 0x%x\n",mmio_read_32(0x0f000000));
    printf(" Write {0x12345678} on 0x0f000000 [sram(64KB)(SBSX)]\n");
    mmio_write_32(0x0f000000, 0x12345678);
    printf(" Read 0x0f000000 is 0x%x\n",mmio_read_32(0x0f000000));
	printf(" =========================================================== \n");
	
	
    printf(" --------------------- [Register Test] --------------------- \n");
}


#include <stdint.h>
__attribute__((noinline))
void delay_busywait(uint32_t iterations)
{
    volatile uint32_t i;
    for (i = 0; i < iterations; ++i) {
        __asm__ __volatile__("nop");
    }
}
void atf_pcie_test()
{
    printf("Start to delay (about 90s)\n");
	delay_busywait(1e5);
	printf("END delay\n");

}
/*
void atf_pcie_test()
{
    printf("[ PCIE-TEST ]\n");
    // printf("Start to delay (about 90s)\n");
	// delay_busywait(1e5);
	// printf("END delay\n");
	printf(" Read 0xf7990000 in bl1_setup is 0x%lx[bl1_main]\n",mmio_read_64(0xf7990000));
	// printf(" Read 0x40000144 in bl1_setup is 0x%lx[bl1_main]\n",mmio_read_64(0x40000144));
	printf(" Read 0x40000000 in bl1_setup is 0x%lx[bl1_main]\n",mmio_read_64(0x40000000));

}
*/