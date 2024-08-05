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

::修改
cd ..\..\ui_resource
::copy file.bin ..\download\watch
::copy *.* ..\download\watch

cd %~dp0

cd ..\..\ui_upgrade
copy *.* ..\download\watch\ui_upgrade
cd %~dp0


..\..\json_to_res.exe json.txt
..\..\md5sum.exe app.bin md5.bin
set /p "themd5=" < "md5.bin"

::修改
::..\..\packres.exe -keep-suffix-case JL.sty JL.res JL.str -n res -o JL
::..\..\packres.exe -keep-suffix-case sidebar.sty sidebar.res sidebar.str sidebar.tab -n res -o sidebar
::..\..\packres.exe -keep-suffix-case watch.sty watch.res watch.str watch.view watch.json -n res -o watch
::..\..\packres.exe -keep-suffix-case watch1.sty watch1.res watch1.str watch1.view watch1.json -n res -o watch1
::..\..\packres.exe -keep-suffix-case watch2.sty watch2.res watch2.str watch2.view watch2.json -n res -o watch2
::..\..\packres.exe -keep-suffix-case watch3.sty watch3.res watch3.str watch3.view watch3.json -n res -o watch3
::..\..\packres.exe -keep-suffix-case watch4.sty watch4.res watch4.str watch4.view watch4.json -n res -o watch4
::..\..\packres.exe -keep-suffix-case watch5.sty watch5.res watch5.str watch5.view watch5.json -n res -o watch5
::..\..\packres.exe -keep-suffix-case F_ASCII.PIX F_GB2312.PIX F_GB2312.TAB ascii.res -n res -o font
::..\..\packres.exe -keep-suffix-case ui.res ui2.res bird_5.bin  -n res -o ui
::..\..\packres.exe -keep-suffix-case file.bin -n res -o ui_user



::echo %1
::cd .\usrfile 
::修改
::..\..\packres.exe -keep-suffix-case .\usrfile\bird_1.bin .\usrfile\preview.bin .\usrfile\1234.jpg -n res -o lvimg
::cd ..

set CHIPKEY=default.key

:: 8MBytes
::..\..\packres.exe -keep-suffix-case F_ASCII.PIX ascii.res -n res -o font
::..\..\fat_comm.exe -pad-backup2 -force-align-fat -out new_res.bin -image-size 8 -filelist  JL sidebar watch3 watch4 font  -remove-empty -remove-bpb -mark-bad-after 0x7e0000 -key %CHIPKEY% -address 0
:: 16MBytes  修改
::..\..\fat_comm.exe -pad-backup2 -force-align-fat -out new_res.bin -image-size 16 -filelist -remove-empty -remove-bpb -mark-bad-after 0xfe0000 -key %CHIPKEY% -address 0
:: 16MBytes
..\..\fat_comm.exe -pad-backup2 -force-align-fat -out new_res.bin -image-size 32 -filelist -remove-empty -remove-bpb -mark-bad-after 0x1FE0000 -key %CHIPKEY% -address 0

::..\..\fat_comm.exe -pad-backup2 -force-align-fat -out new_res2.bin -image-size 16 -filelist  lvmg  -remove-empty -remove-bpb -mark-bad-after 0x800000 -key %CHIPKEY% -address 0

IF %ERRORLEVEL% NEQ 0 goto exit_point

::修改
del /Q res.ori\*
del upgrade.zip
::move JL res.ori\JL
::move watch? res.ori\
::move font res.ori\font

::..\..\packres.exe -n res -o res.bin new_res.bin 0 fat.img 0x640000 -normal
..\..\packres.exe -n res -o res.bin file.bin 0x000000 -normal
::..\..\packres.exe -n res -o res.bin new_res.bin 0x000000 file.bin 0x20000 -normal

::����flash�������ļ���ʾ��Ϊ����0xb00000��ַ����Ҫ������nor_up.ufw
::..\..\packres.exe -n res -o res.bin new_res.bin 0 nor_up.ufw 0xb00000 -normal

::烧录ui资源
::..\..\isd_download.exe -tonorflash -dev br28 -boot 0x120000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin -res ui_upgrade p11_code.bin config.dat tone.cfg eq_cfg_hw.bin -uboot_compress -key %CHIPKEY% -ex_flash res.bin -ex_api_bin user_api.bin
::不烧录ui资源
::..\..\isd_download.exe -tonorflash -dev br28 -boot 0x120000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin -res ui_upgrade p11_code.bin config.dat tone.cfg eq_cfg_hw.bin -uboot_compress -key %CHIPKEY% -ex_api_bin user_api.bin
::-format all
::..\..\isd_download.exe -tonorflash -dev br28 -boot 0x120000 -div8 -wait 300 -uboot uboot.boot -app app.bin cfg_tool.bin -res p11_code.bin config.dat -uboot_compress -key %CHIPKEY% -ex_flash res.bin -ex_api_bin user_api.bin 
::-format all
::-ex_flash res.bin

:: -format all
::-reboot 2500
:: -key xxxx.key

::-format all
::-reboot 100


@rem ɾ����ʱ�ļ�-format all
if exist *.mp3 del *.mp3 
if exist *.PIX del *.PIX
if exist *.TAB del *.TAB
if exist *.res del *.res
if exist *.sty del *.sty
if exist *.str del *.str
if exist *.anim del *.anim
if exist *.view del *.view
if exist *.json del *.json

if exist *.jpg del *.jpg
if exist *.gif del *.gif

::���ʹ������flash�������ļ�������ʽ������������ɵ�update.ufw�в�ֻ�����ֱ���Դ�ļ�������nor_up.ufw�ļ����ݡ�

@rem ���ɹ̼������ļ�
..\..\fw_add.exe -noenc -fw jl_isd.fw  -add ota.bin -type 100 -out jl_isd.fw
@rem �������ýű��İ汾��Ϣ�� FW �ļ���
..\..\fw_add.exe -noenc -fw jl_isd.fw -add script.ver -out jl_isd.fw


..\..\ufw_maker.exe -fw_to_ufw jl_isd.fw
copy jl_isd.ufw update.ufw
del jl_isd.ufw

::..\..\zip.exe -r upgrade.zip res.ori update.ufw
..\..\zip.exe -r upgrade.zip update.ufw

@REM ���������ļ������ļ�
::ufw_maker.exe -chip AC800X %ADD_KEY% -output config.ufw -res bt_cfg.cfg

::IF EXIST jl_693x.bin del jl_693x.bin 


@rem ��������˵��
@rem -format vm        //����VM ����
@rem -format cfg       //����BT CFG ����
@rem -format 0x3f0-2   //��ʾ�ӵ� 0x3f0 �� sector ��ʼ�������� 2 �� sector(��һ������Ϊ16���ƻ�10���ƶ��ɣ��ڶ�������������10����)

:exit_point

ping /n 2 127.1>null
IF EXIST null del null
