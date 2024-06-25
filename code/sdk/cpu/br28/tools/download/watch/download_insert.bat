@echo off

@echo ********************************************************************************
@echo 			SDK BR28			
@echo ********************************************************************************
@echo %date%

cd %~dp0


copy ..\..\script.ver .
copy ..\..\uboot.boot .
copy ..\..\ota.bin .
::copy ..\..\ota_all.bin .
::copy ..\..\ota_nor.bin .
copy ..\..\tone.cfg .
copy ..\..\cfg_tool.bin .
copy ..\..\app.bin .
copy ..\..\br28loader.bin .
copy ..\..\user_api.bin .
copy ..\..\isd_config.ini .
copy ..\..\p11_code.bin .
::copy ..\..\config.dat .
copy ..\..\default.key .
copy ..\..\json.txt .
copy ..\..\eq_cfg_hw.bin .

cd ..\..\ui_resource
copy *.* ..\download\watch

cd %~dp0

cd ..\..\ui_upgrade
copy *.* ..\download\watch\ui_upgrade
cd %~dp0


..\..\json_to_res.exe json.txt
..\..\md5sum.exe app.bin md5.bin
set /p "themd5=" < "md5.bin"

..\..\packres.exe -keep-suffix-case JL.sty JL.res JL.str -n res -o JL
..\..\packres.exe -keep-suffix-case sidebar.sty sidebar.res sidebar.str sidebar.tab -n res -o sidebar
..\..\packres.exe -keep-suffix-case watch.sty watch.res watch.str watch.view watch.json -n res -o watch
..\..\packres.exe -keep-suffix-case watch1.sty watch1.res watch1.str watch1.view watch1.json -n res -o watch1
..\..\packres.exe -keep-suffix-case watch2.sty watch2.res watch2.str watch2.view watch2.json -n res -o watch2
..\..\packres.exe -keep-suffix-case watch3.sty watch3.res watch3.str watch3.view watch3.json -n res -o watch3
..\..\packres.exe -keep-suffix-case watch4.sty watch4.res watch4.str watch4.view watch4.json -n res -o watch4
..\..\packres.exe -keep-suffix-case watch5.sty watch5.res watch5.str watch5.view watch5.json -n res -o watch5
..\..\packres.exe -keep-suffix-case F_ASCII.PIX F_GB2312.PIX F_GB2312.TAB ascii.res -n res -o font

::echo %1

set CHIPKEY=default.key

:: 8MBytes
:: ..\..\fat_comm.exe -pad-backup2 -force-align-fat -out new_res.bin -image-size 8 -filelist  JL sidebar watch watch1 watch2 watch3 watch4 watch5 font  -remove-empty -remove-bpb -mark-bad-after 0x660000 -address 0
:: 16MBytes
..\..\fat_comm.exe -pad-backup2 -force-align-fat -out new_res.bin -image-size 16 -filelist  JL sidebar watch watch1 watch2 watch3 watch4 watch5 font  -remove-empty -remove-bpb -mark-bad-after 0xe60000 -address 0

IF %ERRORLEVEL% NEQ 0 goto exit_point

del /Q res.ori\*
del upgrade.zip
move JL res.ori\JL
move watch? res.ori\
move font res.ori\font

..\..\packres.exe -n res -o res.bin new_res.bin 0 -normal
::外置flash放升级文件，示例为放在0xb00000地址，需要先生成nor_up.ufw
::..\..\packres.exe -n res -o res.bin new_res.bin 0 nor_up.ufw 0xb00000 -normal


..\..\isd_download.exe -tonorflash -dev br28 -boot 0x120000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin -res ui_upgrade p11_code.bin config.dat tone.cfg eq_cfg_hw.bin -uboot_compress -key %CHIPKEY% -ex_api_bin user_api.bin 
::..\..\isd_download.exe -tonorflash -dev br28 -boot 0x120000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin -res p11_code.bin config.dat -uboot_compress -key %CHIPKEY% -ex_flash res.bin -ex_api_bin user_api.bin 
::-format all
::-ex_flash res.bin

:: -format all
::-reboot 2500
:: -key xxxx.key

::-format all
::-reboot 100


@rem 删除临时文件-format all
if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.res del *.res
if exist *.sty del *.sty
if exist *.str del *.str
if exist *.anim del *.anim
if exist *.view del *.view
if exist *.json del *.json


::如果使用外置flash放升级文件处理方式，下面操作生成的update.ufw中不只包含手表资源文件，还有nor_up.ufw文件内容。

@rem 生成固件升级文件
..\..\fw_add.exe -noenc -fw jl_isd.fw  -add ota.bin -type 100 -out jl_isd.fw
@rem 添加配置脚本的版本信息到 FW 文件中
..\..\fw_add.exe -noenc -fw jl_isd.fw -add script.ver -out jl_isd.fw


..\..\ufw_maker.exe -fw_to_ufw jl_isd.fw
copy jl_isd.ufw update.ufw
del jl_isd.ufw

::..\..\zip.exe -r upgrade.zip res.ori update.ufw
..\..\zip.exe -r upgrade.zip update.ufw

@REM 生成配置文件升级文件
::ufw_maker.exe -chip AC800X %ADD_KEY% -output config.ufw -res bt_cfg.cfg

::IF EXIST jl_693x.bin del jl_693x.bin 


@rem 常用命令说明
@rem -format vm        //擦除VM 区域
@rem -format cfg       //擦除BT CFG 区域
@rem -format 0x3f0-2   //表示从第 0x3f0 个 sector 开始连续擦除 2 个 sector(第一个参数为16进制或10进制都可，第二个参数必须是10进制)

:exit_point

ping /n 2 127.1>null
IF EXIST null del null
::pause
