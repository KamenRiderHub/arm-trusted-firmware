反汇编DEBUG文件里的.dll文件

```
aarch64-linux-gnu-objdump -m aarch64 -D /home/linux8007/edk2/edk2-rls-1108/Build/ArmSample/DEBUG_GCC5/AARCH64/MdeModulePkg/Core/Dxe/DxeMain/DEBUG/DxeCore.dll > /home/linux8007/edk2/edk2-rls-1108/Build/ArmSample/DEBUG_GCC5/AARCH64/MdeModulePkg/Core/Dxe/DxeMain/DEBUG/t-dxe-9.asm
```



bin转txt：

```
xxd -p -c 1 bl1-pcie3.bin | sed 's/\(..\)/\1 /g; s/ $//' > bl1-pcie3.txt
```





gArmTokenSpaceGuid.PcdSystemMemorySize|0x70000000（对齐64MB）

[UEFI_BP] Enter Buffer=F7990000 Length=327680 Value=0xAF



gArmTokenSpaceGuid.PcdSystemMemorySize|0x6f000000（没对齐64MB）

[UEFI_BP] Enter Buffer=F6801000 Length=18D000 Value=0xAF



gArmTokenSpaceGuid.PcdSystemMemorySize|0x68000000

[UEFI_BP] Pointer64 is changed to 0xEFA10000, Length is changed to 20000



gArmTokenSpaceGuid.PcdSystemMemorySize|0x8000000







mov	w0, #0x86a0
movk	w0, #0x1, lsl #16



mov	x1, #0x40000000
ldr	x0, [x1]

bl	atf_testpoint_printReg_hex

