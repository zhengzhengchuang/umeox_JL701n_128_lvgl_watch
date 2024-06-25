
























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

%OBJDUMP% -D -address-mask=0x1ffffff -print-dbg sdk.elf > sdk.lst
%OBJCOPY% -O binary -j .text %ELFFILE% text.bin
%OBJCOPY% -O binary -j .data %ELFFILE% data.bin
%OBJCOPY% -O binary -j .data_code %ELFFILE% data_code.bin
%OBJCOPY% -O binary -j .overlay_aec %ELFFILE% aec.bin
%OBJCOPY% -O binary -j .overlay_aac %ELFFILE% aac.bin
%OBJCOPY% -O binary -j .overlay_aptx %ELFFILE% aptx.bin
%OBJCOPY% -O binary -j .ps_ram_data_code %ELFFILE% psram_data_code.bin

%OBJCOPY% -O binary -j .common %ELFFILE% common.bin

%OBJDUMP% -section-headers -address-mask=0x1ffffff %ELFFILE%

set OBJSIZEDUMP=C:\JL\pi32\bin\llvm-objsizedump.exe
%OBJSIZEDUMP% -lite -skip-zero -enable-dbg-info %ELFFILE% | sort > symbol_tbl.txt


copy /b text.bin + data.bin + data_code.bin + aec.bin + aac.bin + aptx.bin + common.bin + psram_data_code.bin app.bin

del common.bin
del aptx.bin
del aac.bin
del aec.bin
del data_code.bin
del data.bin
del text.bin
del psram_data_code.bin
copy .\effect_file\music_base.bin eq_cfg_hw.bin






call download/watch/download.bat
