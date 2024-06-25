













@echo off
Setlocal enabledelayedexpansion
@echo ********************************************************************************
@echo SDK BR28
@echo ********************************************************************************
@echo %date%


cd /d %~dp0

set OBJDUMP=C:\JL\pi32\bin\llvm-objdump.exe
set OBJCOPY=C:\JL\pi32\bin\llvm-objcopy.exe
set ELFFILE=sdk.elf

%OBJDUMP% -D -address-mask=0x1ffffff -print-dbg %ELFFILE% > %ELFFILE%.lst
%OBJCOPY% -O binary -j .text %ELFFILE% text.bin
%OBJCOPY% -O binary -j .data %ELFFILE% data.bin
%OBJCOPY% -O binary -j .data_code %ELFFILE% data_code.bin
%OBJCOPY% -O binary -j .overlay_aec %ELFFILE% aec.bin
%OBJCOPY% -O binary -j .overlay_aac %ELFFILE% aac.bin
%OBJCOPY% -O binary -j .overlay_aptx %ELFFILE% aptx.bin

%OBJCOPY% -O binary -j .common %ELFFILE% common.bin

%OBJDUMP% -section-headers -address-mask=0x1ffffff %ELFFILE%
REM %OBJDUMP% -t %ELFFILE% > symbol_tbl.txt

copy /b text.bin + data.bin + data_code.bin + aec.bin + aac.bin + aptx.bin + common.bin app.bin

copy text.bin user_api.bin

del common.bin
del aptx.bin
del aac.bin
del aec.bin
del data_code.bin
del data.bin
del text.bin


REM call download/watch/download.bat
if exist "..\..\..\..\..\sdk\cpu\br28\tools\" (
    copy user_api.bin     	..\..\..\..\..\sdk\cpu\br28\tools\user_api.bin
	copy uboot.boot     	..\..\..\..\..\sdk\cpu\br28\tools\uboot.boot
	copy ota.bin 			..\..\..\..\..\sdk\cpu\br28\tools\ota.bin
	copy ota_debug.bin 		..\..\..\..\..\sdk\cpu\br28\tools\ota_debug.bin
	copy user_api.bin     	..\..\..\..\..\sdk\cpu\br28\tools\download\watch\user_api.bin
)

