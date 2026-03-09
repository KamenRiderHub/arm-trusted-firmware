编译：

make realclean

make PLAT=fvp all fip BL33=myuefi/ARMUEFI-m15.fd

可以加“ DEBUG=1”



反汇编：

aarch64-linux-gnu-objdump -m aarch64 -D /home/linux8007/atf_v2.7/build/fvp/release/bl1/bl1.elf > /home/linux8007/atf_v2.7/build/fvp/release/t290.asm



aarch64-linux-gnu-objdump -m aarch64 -D /home/linux8007/atf_v2.7/build/fvp/release/bl2/bl2.elf > /home/linux8007/atf_v2.7/build/fvp/release/t290-2.asm



aarch64-linux-gnu-objdump -m aarch64 -D /home/linux8007/atf_v2.7/build/fvp/release/bl31/bl31.elf > /home/linux8007/atf_v2.7/build/fvp/release/t290-3.asm



转软模拟用的.txt文件

xxd -p -c 1 bl1-pcie2.bin | sed 's/\(..\)/\1 /g; s/ $//' > bl1-pcie2.txt