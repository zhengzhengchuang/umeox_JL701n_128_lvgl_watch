@echo off

@echo ********************************************************************************
@echo 			SDK BR28			
@echo ********************************************************************************
@echo %date%

cd %~dp0


::if exist uboot.boot del uboot.boot
::type uboot.bin > uboot.boot

::copy rslib.bin .\res\
::copy bt_cfg.bin .\res\
::copy dspboot.bin .\res\
::copy sbc_dec.bin .\res\
::copy mp3_dec_lib.bin .\res\
::copy aac_dec_dsp.bin .\res\
::copy eq.bflt .\res\
::copy commproc.bflt .\res\
::copy config.cfg .\res\
::copy convert.bflt .\res\

::isd_download.exe -tonorflash -dev br28 -boot 0x120000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin  -res tone.cfg p11_code.bin -uboot_compress -ex_flash res.bin

packres.exe -keep-suffix-case clock2.bin clock2.rle clock3.bin clock4.bin clock4.rle clock5.rle -n res -o clock

fat_comm.exe -pad-backup -out new_res.bin -image-size 10 -filelist clock clock2.rle -remove-empty -remove-bpb

packres.exe -n res -o res.bin new_res.bin 0 -normal


isd_download.exe -tonorflash -dev br28 -boot 0x120000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin  -res tone.cfg p11_code.bin bg.res hour.res red.rle dial1bk.rle clock2.rle dial3bk.rle 48x49.rle 48x49.bin 128x32.bin 127x48.bin hour.rle hour.bin -uboot_compress -ex_flash_enc res.bin



::-format all
::-ex_flash res.bin
::isd_download_noenc.exe -tonorflash -dev br28 -boot 0x120000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin  -res tone.cfg p11_code.bin -uboot_compress

:: -format all
::-reboot 2500
:: -key 002伦茨AC690X-4A30.key

::-format all
::-reboot 100




@rem 删除临时文件-format all
if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
::if exist *.res del *.res
if exist *.sty del *.sty



@rem 生成固件升级文件
fw_add.exe -noenc -fw jl_isd.fw  -add ota.bin -type 100 -out jl_isd.fw
@rem 添加配置脚本的版本信息到 FW 文件中
fw_add.exe -noenc -fw jl_isd.fw -add script.ver -out jl_isd.fw


ufw_maker.exe -fw_to_ufw jl_isd.fw
copy jl_isd.ufw update.ufw
del jl_isd.ufw

@REM 生成配置文件升级文件
::ufw_maker.exe -chip AC800X %ADD_KEY% -output config.ufw -res bt_cfg.cfg

::IF EXIST jl_693x.bin del jl_693x.bin 


@rem 常用命令说明
@rem -format vm        //擦除VM 区域
@rem -format cfg       //擦除BT CFG 区域
@rem -format 0x3f0-2   //表示从第 0x3f0 个 sector 开始连续擦除 2 个 sector(第一个参数为16进制或10进制都可，第二个参数必须是10进制)

ping /n 2 127.1>null
IF EXIST null del null
pause
