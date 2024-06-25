rem @echo off

@echo *****************************************************************
@echo SDK BR28 P11
@echo *****************************************************************
@echo %date%

cd %~dp0

set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe
set OBJSIZEDUMP=C:\JL\pi32\bin\llvm-objsizedump.exe
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe
set BANKLINK=.\BankLink.exe
set ELFFILE=p11.elf

REM %OBJDUMP% -D -address-mask=0x1ffffff -print-imm-hex -print-dbg %ELFFILE% > p11.lst

%OBJCOPY% -O binary -j .text %ELFFILE% text.bin
%OBJCOPY% -O binary -j .overlay_bank_demo_init %ELFFILE% bank_demo_init.bin
%OBJCOPY% -O binary -j .overlay_bank_demo_normal0 %ELFFILE% bank_demo_normal0.bin
%OBJCOPY% -O binary -j .overlay_bank_demo_normal1 %ELFFILE% bank_demo_normal1.bin
%OBJCOPY% -O binary -j .overlay_bank_demo_normal2 %ELFFILE% bank_demo_normal2.bin

%OBJCOPY% -O binary -j .overlay_bank_sensor_init %ELFFILE% bank_sensor_init.bin
%OBJCOPY% -O binary -j .overlay_bank_sensor_normal0 %ELFFILE% bank_sensor_normal0.bin
%OBJCOPY% -O binary -j .overlay_bank_sensor_normal1 %ELFFILE% bank_sensor_normal1.bin



%OBJDUMP% -section-headers -address-mask=0x1ffffff %ELFFILE%
%OBJDUMP% -t %ELFFILE% > symbol_tbl.txt

%OBJSIZEDUMP% -dump-stack-size -enable-dbg-info %ELFFILE% > dump_stack_size.txt
%OBJSIZEDUMP% -dump-function-call -enable-dbg-info %ELFFILE% > dump_func_call.txt

set bank_files=0x80 text.bin 

for %%a in (bank_demo_init.bin) do if %%~za gtr 0 (
set bank_files=%bank_files% 0xAA55AA55 bank_demo_init.bin)

for %%a in (bank_demo_normal0.bin) do if %%~za gtr 0 (
set bank_files=%bank_files% 0xAA55AA55 bank_demo_normal0.bin)

for %%a in (bank_demo_normal1.bin) do if %%~za gtr 0 (
set bank_files=%bank_files% 0xAA55AA55 bank_demo_normal1.bin)

for %%a in (bank_sensor_init.bin) do if %%~za gtr 0 (
set bank_files=%bank_files% 0xAA55AA55 bank_sensor_init.bin)

for %%a in (bank_sensor_normal0.bin) do if %%~za gtr 0 (
set bank_files=%bank_files% 0xAA55AA55 bank_sensor_normal0.bin)

for %%a in (bank_sensor_normal1.bin) do if %%~za gtr 0 (
set bank_files=%bank_files% 0xAA55AA55 bank_sensor_normal1.bin)

for %%a in (bank_sys_init.bin) do if %%~za gtr 0 (
set bank_files=%bank_files% 0xAA55AA55 bank_sys_init.bin)

for %%a in (bank_sys_normal0.bin) do if %%~za gtr 0 (
set bank_files=%bank_files% 0xAA55AA55 bank_sys_normal0.bin)

echo %bank_files%

%BANKLINK% %bank_files% p11_bank_code.bin

if exist "..\..\..\..\sdk\cpu\br28\tools\" (
    copy .\p11_bank_code.bin     ..\..\..\..\sdk\cpu\br28\tools\p11_code.bin
)

::pause

