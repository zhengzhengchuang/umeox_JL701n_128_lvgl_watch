// *INDENT-OFF*
@echo off

@echo ********************************************************************************
@echo 			BR28 OTA LOADER			
@echo ********************************************************************************
@echo %date%

cd /d %~dp0

set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe
set TARGET_ELF=ota_loader.elf

set OTA_LOADER_PACKER=ota_loader.exe
set UBOOT_PACKER=uboot_package.exe

echo %TARGET_ELF%

set PROJECT_TYPE=OTA_LOADER_TYPE

%OBJCOPY% -O binary -j .text %TARGET_ELF% %PROJECT_TYPE%_bank0.bin
%OBJCOPY% -O binary -j .ver_tag %TARGET_ELF%  ota_loader_ver.bin
%OBJCOPY% -O binary -j .version %TARGET_ELF% ota_loader.version
%OBJDUMP% -D -address-mask=0x7ffffff -print-imm-hex -print-dbg -mcpu=r3 %TARGET_ELF% > ota_loader.lst
%OBJDUMP% -section-headers -address-mask=0x7ffffff %TARGET_ELF% 
REM %OBJDUMP% -t %TARGET_ELF% >  symbol_tbl.txt

echo %PROJECT_TYPE%

copy %PROJECT_TYPE%_bank0.bin %PROJECT_TYPE%.bin

%OTA_LOADER_PACKER% 0x177000 %PROJECT_TYPE%.bin compression ram

%UBOOT_PACKER% --info ota_loader.version --uboot ota.bin --output ota.bin


